#include "hemerageneratorsbasecompiler.h"

#include "hemerageneratorsbasegenerator_p.h"
#include "hemerageneratorscompilerplugin_p.h"

#include <HemeraCore/Literals>
#include <HemeraGenerators/CompilerPlugin>
#include <HemeraQml/QmlEngine>

#include <QtCore/QCommandLineParser>
#include <QtCore/QTimer>

#include <QtQml/QQmlComponent>
#include <QtQml/QQmlEngine>

#include <hemeractconfig.h>
class toSt;

namespace Hemera {
namespace Generators {

class CompilerConfigurationData : public QSharedData
{
public:
    CompilerConfigurationData() : valid(false), allowMultilink(false), cleanup(false), cleanupAllPrevious(false)
                                , inTreeCompile(false), invokeUserManager(false), debugSupport(false) { }
    CompilerConfigurationData(const QStringList &arguments);
    CompilerConfigurationData(const CompilerConfigurationData &other)
        : QSharedData(other), valid(other.valid), allowMultilink(other.allowMultilink), cleanup(other.cleanup), cleanupAllPrevious(other.cleanupAllPrevious)
        , inTreeCompile(other.inTreeCompile), invokeUserManager(other.invokeUserManager), debugSupport(other.debugSupport), files(other.files)
        , prefix(other.prefix) { }
    ~CompilerConfigurationData() { }

    bool valid;

    bool allowMultilink;
    bool cleanup;
    bool cleanupAllPrevious;
    bool inTreeCompile;
    bool invokeUserManager;
    bool debugSupport;
    QStringList files;
    QString prefix;
};

CompilerConfigurationData::CompilerConfigurationData(const QStringList& arguments)
{
    // Populate from arguments
    QCommandLineParser parser;
    parser.setApplicationDescription(CompilerConfiguration::tr("Gravity Compiler"));
    parser.addHelpOption();
    parser.addVersionOption();

    parser.addPositionalArgument(QStringLiteral("files"), CompilerConfiguration::tr("Files to compile."));

    QCommandLineOption prefixOption(QStringList() << QStringLiteral("p") << QStringLiteral("prefix"),
                                    CompilerConfiguration::tr("Prefix for the compiler."),
                                    QStringLiteral("prefix"));
    parser.addOption(prefixOption);
    QCommandLineOption allowMultilinkOption(QStringList() << QStringLiteral("m") << QStringLiteral("allow-multilink"),
                                            CompilerConfiguration::tr("Allow rewriting symlinks."));
    parser.addOption(allowMultilinkOption);
    QCommandLineOption cleanupOption(QStringList() << QStringLiteral("c") << QStringLiteral("cleanup"),
                                     CompilerConfiguration::tr("Cleanup previous compilations."));
    parser.addOption(cleanupOption);
    QCommandLineOption cleanupAllPreviousOption(QStringList() << QStringLiteral("cleanup-all-previous"),
                                     CompilerConfiguration::tr("Cleanup every previous compilation. WARNING: This might compromise your system!"));
    parser.addOption(cleanupAllPreviousOption);
    QCommandLineOption inTreeCompileOption(QStringList() << QStringLiteral("in-tree-compile"),
                                           CompilerConfiguration::tr("Running compiler from Gravity's tree"));
    parser.addOption(inTreeCompileOption);
    QCommandLineOption invokeUserManagerOption(QStringList() << QStringLiteral("invoke-user-manager"),
                                           CompilerConfiguration::tr("Invoke user manager right after. Quite useful when developing."));
    parser.addOption(invokeUserManagerOption);
    QCommandLineOption debugSupportOption(QStringList() << QStringLiteral("debug-support"),
                                          CompilerConfiguration::tr("Compiles support for debug targets and orbits. Makes sense only if compiling an Orbital Application."));
    parser.addOption(debugSupportOption);

    parser.process(arguments);

    // Populate settings
    allowMultilink = parser.isSet(allowMultilinkOption);
    cleanup = parser.isSet(cleanupOption);
    cleanupAllPrevious = parser.isSet(cleanupAllPreviousOption);
    inTreeCompile = parser.isSet(inTreeCompileOption);
    invokeUserManager = parser.isSet(invokeUserManagerOption);
    debugSupport = parser.isSet(debugSupportOption);
    files = parser.positionalArguments();

    if (files.isEmpty()) {
        valid = false;
    }

    prefix = parser.value(prefixOption);
    if (prefix.endsWith(QLatin1Char('/'))) {
        prefix.chop(1);
    }

    valid = true;
}

CompilerConfiguration::CompilerConfiguration()
    : d(new CompilerConfigurationData)
{
}

CompilerConfiguration::CompilerConfiguration(const QStringList& arguments)
    : d(new CompilerConfigurationData(arguments))
{
}

CompilerConfiguration::CompilerConfiguration(const CompilerConfiguration& other)
    : d(other.d)
{
}

CompilerConfiguration::~CompilerConfiguration()
{
}

bool CompilerConfiguration::isValid() const
{
    return d->valid;
}

CompilerConfiguration& CompilerConfiguration::operator=(const CompilerConfiguration& rhs)
{
    if (this==&rhs) {
        // Protect against self-assignment
        return *this;
    }

    d = rhs.d;
    return *this;
}

bool CompilerConfiguration::operator==(const CompilerConfiguration& other) const
{
    return other.files() == d->files && other.prefix() == d->prefix;
}

bool CompilerConfiguration::allowMultilink() const
{
    return d->allowMultilink;
}

bool CompilerConfiguration::cleanup() const
{
    return d->cleanup;
}

bool CompilerConfiguration::cleanupAllPrevious() const
{
    return d->cleanupAllPrevious;
}

bool CompilerConfiguration::debugSupport() const
{
    return d->debugSupport;
}

QStringList CompilerConfiguration::files() const
{
    return d->files;
}

bool CompilerConfiguration::inTreeCompile() const
{
    return d->inTreeCompile;
}

bool CompilerConfiguration::invokeUserManager() const
{
    return d->invokeUserManager;
}

QString CompilerConfiguration::prefix() const
{
    return d->prefix;
}


class BaseCompilerPrivate : public BaseGeneratorPrivate
{
public:
    BaseCompilerPrivate(bool terminateOnCompletion, BaseCompiler *q)
        : BaseGeneratorPrivate(terminateOnCompletion, q) {}
    virtual ~BaseCompilerPrivate() {}

    Q_DECLARE_PUBLIC(BaseCompiler)

    virtual void initHook() Q_DECL_OVERRIDE;

    QStringList arguments;
    CompilerConfiguration configuration;

    QQmlEngine *qmlEngine;
    int filesLeft;

    QHash< QString, QObject* > configurationObjects;

    // Q_PRIVATE_SLOTS
    void compileNext();
};

void BaseCompilerPrivate::initHook()
{
    Q_Q(BaseCompiler);

    // Generate configuration
    configuration = CompilerConfiguration(arguments);

    if (!configuration.isValid()) {
        q->setInitError(Literals::literal(Literals::Errors::badRequest()),
                        BaseCompiler::tr("Wrong configuration given to compiler!"));
        return;
    }

    qmlEngine = Hemera::qmlEngine(q);
    qmlEngine->setOutputWarningsToStandardError(false);

    QObject::connect(qmlEngine, &QQmlEngine::warnings, [q] (const QList<QQmlError> &warnings) {
            QString errorString;
            for (const QQmlError &warning : warnings) {
                errorString.append(QStringLiteral("\t%1\n").arg(warning.toString()));
            }
            q->setInitError(Hemera::Literals::literal(Hemera::Literals::Errors::declarativeError()),
                            BaseCompiler::tr("The following problems occurred when parsing your configuration files: %1").arg(errorString));
    });

    filesLeft = configuration.files().count();

    // Load all the files
    for (const QString &file : configuration.files()) {
        if (!QFile::exists(file)) {
            q->setInitError(Hemera::Literals::literal(Hemera::Literals::Errors::notFound()),
                            BaseCompiler::tr("File %1 does not exist").arg(file));
            return;
        }

        QQmlComponent *component = new QQmlComponent(qmlEngine, QUrl::fromLocalFile(file), QQmlComponent::Asynchronous, q);

        auto onComponentStatusChanged = [this, q, component, file] (QQmlComponent::Status status) {
            if (status == QQmlComponent::Error) {
                q->setInitError(Hemera::Literals::literal(Hemera::Literals::Errors::declarativeError()),
                                [component] () -> QString {
                                    QString result;
                                    for (const QQmlError &error : component->errors()) {
                                        result.append(error.toString());
                                        result.append(QLatin1Char('\n'));
                                    }
                                    return result; } ());
                component->deleteLater();
            } else if (status == QQmlComponent::Ready) {
                QObject *configurationObject = component->create();

                if (configurationObject == nullptr) {
                    q->setInitError(Hemera::Literals::literal(Hemera::Literals::Errors::declarativeError()),
                                    BaseCompiler::tr("Could not create component"));
                    return;
                }

                configurationObjects.insert(file, configurationObject);
                --filesLeft;
                if (!filesLeft) {
                    q->setThingsToDo(configurationObjects.size());
                    Hemera::Generators::BaseGeneratorPrivate::initHook();
                }
            }
        };

        if (component->isLoading()) {
            QObject::connect(component, &QQmlComponent::statusChanged, onComponentStatusChanged);
        } else {
            onComponentStatusChanged(component->status());
        }
    }
}

void BaseCompilerPrivate::compileNext()
{
    if (configurationObjects.isEmpty()) {
        return;
    }

    Q_Q(BaseCompiler);

    QString file = configurationObjects.keys().first();
    QObject *object = configurationObjects.take(file);

    std::cout << BaseCompiler::tr("Compiling %1...").arg(file).toStdString() << std::endl;

    // Find the correct plugin
    const char *className = object->metaObject()->className();
    Hemera::Operation *op = nullptr;
    for (CompilerPlugin *plugin : q->plugins()) {
        op = plugin->operationForClassName(className, object, configuration);
        if (op != nullptr) {
            break;
        }
    }

    if (op == nullptr) {
        std::cerr << BaseCompiler::tr("No suitable plugins found! Either your file is not compatible with this Hemera version or your installation is incomplete.").toStdString() << std::endl;
        q->oneThingLessToDo();
        QTimer::singleShot(0, q, SLOT(compileNext()));
        return;
    }

    QObject::connect(op, &Hemera::Operation::finished, [this, q, op] {
        if (op->isError()) {
            std::cerr << BaseCompiler::tr("ERROR!").toStdString() << " " << op->errorName().toLatin1().constData()
                      << op->errorMessage().toLatin1().constData() << std::endl;
        } else {
            std::cout << BaseCompiler::tr("Success!").toStdString() << std::endl;
        }

        q->oneThingLessToDo();
        if (!configurationObjects.isEmpty()) {
            QTimer::singleShot(0, q, SLOT(compileNext()));
        }
    });
}

BaseCompiler::BaseCompiler(const QStringList &arguments, bool terminateOnCompletion, QObject* parent)
    : BaseGenerator(*new BaseCompilerPrivate(terminateOnCompletion, this), parent)
    , GeneratorPluginManager< Hemera::Generators::CompilerPlugin, Hemera::Generators::BaseCompiler >(QLatin1String(StaticConfig::hemeraCompilerPluginsDir()), this)
{
    Q_D(BaseCompiler);
    d->arguments = arguments;
}

BaseCompiler::~BaseCompiler()
{
}

QQmlEngine* BaseCompiler::qmlEngine()
{
    Q_D(BaseCompiler);
    return d->qmlEngine;
}

CompilerConfiguration BaseCompiler::configuration() const
{
    Q_D(const BaseCompiler);
    return d->configuration;
}

void BaseCompiler::startCompilation()
{
    QTimer::singleShot(0, this, SLOT(compileNext()));
}

}
}

// For Q_PRIVATE_SLOTS
#include "moc_hemerageneratorsbasecompiler.cpp"
