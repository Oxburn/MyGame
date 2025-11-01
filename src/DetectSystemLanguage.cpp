#include <windows.h>
#include <string>
#include <locale>

int DetectSystemLanguageFallback()
{
    std::string name = std::locale("").name(); // ex: "fr_FR.UTF-8" ou "en_US.UTF-8"

    if (name.rfind("fr", 0) == 0) return 0; // Français
    if (name.rfind("en", 0) == 0) return 1; // Anglais
    if (name.rfind("es", 0) == 0) return 2; // Espagnol
    return 1; // Anglais
}

int DetectSystemLanguage()
{
    LANGID langId = GetUserDefaultUILanguage();  // ex: 0x040C pour FR, 0x0409 pour EN

    switch (PRIMARYLANGID(langId)) {
        case LANG_FRENCH:
            return 0; // Français
        case LANG_ENGLISH:
            return 1; // Anglais
        case LANG_SPANISH:
            return 2; // Espagnol
        default:
            return DetectSystemLanguageFallback(); // Fallback → Linux/MacOS
    }
}