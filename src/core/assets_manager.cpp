#include "assets_manager.h"

#include <iostream>
#include <vector>
#include <fstream>

NAMEPSACE_AH2D_START;

namespace AssetsManager
{
    bool init(const char* argv0)
    {
        const char* org_name = "h2d";
        const char* app_name = "advanced_Horizon2D";

        std::string pakFilename = "assets.pak";

        if (!PHYSFS_init(NULL))
        {
            std::cerr << "ERREUR: Échec de l'initialisation de PhysFS: " << PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()) << std::endl;
            return false;
        }
        //Définir le répertoire où PhysFS peut écrire des fichiers (sauvegardes, logs, etc.)
        PHYSFS_setWriteDir(PHYSFS_getPrefDir(org_name, app_name));

        // Tente de monter le fichier pak. L'archive est ajoutée à la recherche des fichiers.
        if (!PHYSFS_mount(pakFilename.c_str(), "/", 1))
        {
            // Le 1 signifie "prepend", de sorte que ce fichier soit recherché en premier.
            std::cerr << "ERREUR: Échec du montage de l'archive: " << PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()) << std::endl;
            return false;
        }
        return true;
    }

    void deinit()
    {
        //Libere les ressources
        if (!PHYSFS_deinit())
        {
            std::cerr << "Erreur PhysFS lors de la désinitialisation: " << PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()) << std::endl;
        }
    }


    std::vector<char> load_asset(const std::string& packedPath)
    {
        //1. Ouvrir le fichier via PhysFS.
        //PhysFS cherche automatiquement l'ID (packedPath) dans tous les chemins montés.
        PHYSFS_File *file = PHYSFS_openRead(packedPath.c_str());

        if (!file)
        {
            //Gérer l'erreur si le fichier n'est pas trouvé ou ne peut pas être ouvert.
            std::cerr << "Erreur PhysFS lors de l'ouverture de l'asset " << packedPath << " : " << PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()) << std::endl;
            return {}; //Retourne un vecteur vide en cas d'échec.
        }

        //2. Déterminer la taille du fichier.
        PHYSFS_sint64 file_size = PHYSFS_fileLength(file);

        if (file_size <= 0)
        {
            std::cerr << "Erreur: Taille de fichier invalide ou vide pour l'asset " << packedPath << "." << std::endl;
            PHYSFS_close(file);
            return {};
        }

        // 3. Allouer le tampon (buffer) en mémoire.
        // std::vector gère l'allocation mémoire pour nous.
        std::vector<char> buffer((size_t)file_size);

        // 4. Lire toutes les données du fichier dans le buffer.
        // buffer.data() donne le pointeur vers la zone mémoire du vecteur.
        PHYSFS_sint64 read_count = PHYSFS_readBytes(file, buffer.data(), (PHYSFS_uint64)file_size);

        // 5. Fermer le fichier.
        PHYSFS_close(file);

        // 6. Vérifier si la lecture a été complète.
        if (read_count != file_size)
        {
            std::cerr << "Erreur: Lecture incomplète pour l'asset " << packedPath << ". "
                      << "Bytes lus: " << read_count << " / " << file_size << std::endl;
            return {}; // Retourne un vecteur vide en cas d'échec.
        }

        // 7. Retourner le vecteur contenant les données brutes de l'asset.
        return buffer;
    }

    PHYSFS_sint64 get_asset_size(const std::string& packedPath)
    {
        PHYSFS_File *file = PHYSFS_openRead(packedPath.c_str());

        if (!file)
        {
            // L'asset n'a pas été trouvé ou ne peut pas être ouvert.
            std::cerr << "Erreur PhysFS [get_asset_size]: Impossible d'ouvrir l'asset " << packedPath << " : " << PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()) << std::endl;
            return -1;
        }

        // Récupérer la taille du fichier (en octets)
        PHYSFS_sint64 file_size = PHYSFS_fileLength(file);

        // Fermer le descripteur de fichier immédiatement.
        // C'est très important pour libérer la ressource !
        PHYSFS_close(file);

        if (file_size < 0)
        {
            // Erreur lors de la détermination de la taille (ce cas est rare si l'ouverture a réussi).
            std::cerr << "Erreur PhysFS [get_asset_size]: Taille invalide pour l'asset " << packedPath << "." << std::endl;
            return -1;
        }

        return file_size;
    }


    bool file_exists(const std::string& packedPath)
    {
        return PHYSFS_exists(packedPath.c_str());
    }


    std::vector<std::vector<char>> load_assets_from_pak(const std::vector<char>& openedPakFile)
    {
        std::vector<std::vector<char>> assets;

        // Monter le pak en mémoire
        if (!PHYSFS_mountMemory(
                openedPakFile.data(),              // données
                openedPakFile.size(),              // taille
                nullptr,                           // pas de callback pour libérer
                "virtual.pak",                     // nom interne du pak
                "/",                               // point de montage virtuel
                1))                    // ajouter au search path
        {
            std::cerr << "Erreur montage pak : " << PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()) << std::endl;
            return assets;
        }

        // Parcourir le dossier racine
        const char* dir = "/";
        char **rc = PHYSFS_enumerateFiles(dir);

        for (char **i = rc; *i != nullptr; i++)
        {
            std::string fullPath = std::string(dir) + *i;

            // Vérifie que ce n'est pas un dossier
            PHYSFS_Stat stat;
            if (PHYSFS_stat(fullPath.c_str(), &stat) != 0)
            {
                if (stat.filetype == PHYSFS_FILETYPE_REGULAR)
                {
                    // Charge le fichier et ajoute au vector
                    assets.push_back(load_asset(fullPath));
                }
            }
        }

        PHYSFS_freeList(rc);

        // Démonter le pak mémoire si nécessaire
        PHYSFS_unmount("/");

        return assets;
    }

    std::vector<std::string> find_assets_from_path(const std::string &packedPath)
    {
        std::vector<std::string> files;

        char** rc = PHYSFS_enumerateFiles(packedPath.c_str());
        for (char **i = rc; *i != nullptr; i++)
        {
            std::string relativePath = *i;
            std::string fullCheck = packedPath + "/" + relativePath;

            PHYSFS_Stat stat;
            if (PHYSFS_stat(fullCheck.c_str(), &stat) != 0) // succès
            {
                if (stat.filetype == PHYSFS_FILETYPE_REGULAR)
                {
                    files.push_back(fullCheck);
                }
            }
            else
            {
                std::cerr << "Erreur PHYSFS_stat sur " << fullCheck << std::endl;
            }
        }

        PHYSFS_freeList(rc);
        return files;
    }
}

NAMEPSACE_AH2D_END;