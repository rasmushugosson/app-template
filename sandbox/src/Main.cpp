#include "pch.h"

#include "App.h"
#include "Log.h"

#include <cstdio>
#include <exception>

void Run()
{
    ae::Logger::Get().AddConsoleSink("Console", ae::LogSinkConsoleKind::STDOUT, AE_TRACE, AE_WARNING);
    ae::Logger::Get().AddConsoleSink("Errors", ae::LogSinkConsoleKind::STDERR, AE_ERROR);

    AE_LOG(AE_INFO, "App starting...");
    AE_LOG_NEWLINE();

    try
    {
        App app;

        app.Init();
        app.Run();
        app.CleanUp();
    }

    catch (const std::exception &e)
    {
        AE_LOG(AE_ERROR, "{}", e.what());
    }

    AE_LOG_NEWLINE();
    AE_LOG(AE_INFO, "App closing...");
}

int main()
{
    try
    {
        Run();
    }

    catch (...)
    {
        std::fputs("Unexpected error thrown in Run()", stderr);
    }
}
