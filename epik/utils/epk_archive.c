/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2003-2008                                                **
**  University of Tennessee, Innovative Computing Laboratory               **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include "epk_conf.h"
#include "elg_error.h"
#include "epk_archive.h"

#define EPK_NAME       "epik"
#define EPK_DIR_PREFIX "epik_"

typedef struct {
    epk_type_t type;
    char name[5];
    char suffix[5];
    char description[13];
} epk_t;

/* the index/order of types in the table must match enum epk_type_t */

static epk_t epk_types[] = {
    { EPK_TYPE_DIR,      "",     "", "measurement"  },
    { EPK_TYPE_ESD,   "ESD",  "esd", "definitions"  },
    { EPK_TYPE_ETM,   "ETM",  "etm", "profile sum"  },
    { EPK_TYPE_ELG,   "ELG",  "elg", "event trace"  },
    { EPK_TYPE_EMP,   "EMP",  "map", "id mappings"  },
    { EPK_TYPE_OTF,   "OTF",  "otf", "OTF trace"    },
    { EPK_TYPE_CUBE, "CUBE", "cube", "CUBE summary" },
    { EPK_TYPE_SION, "SION", "sion", "SION traces"  },
    { EPK_TYPE_MAX,      "",     "", "NOT DEFINED"  },
};

static char epk_archive_dir[PATH_MAX] = { '\0' };
static char epk_archive_cwd[PATH_MAX] = { '\0' };

char* epk_archive_get_name (void)
{
    if (epk_archive_dir[0] == '\0') { /* derive from configuration */
        snprintf(epk_archive_dir, PATH_MAX, "%s/%s%s",
                 epk_get(EPK_GDIR), EPK_DIR_PREFIX, epk_get(EPK_TITLE));
    }
    return epk_archive_dir;
}

char* epk_archive_set_name (const char* name)
{
    if (name && name[0]) { /* use provided name */
        char* epik_dir = epk_archive_filename (EPK_TYPE_DIR, name);
        if (strlen(epik_dir)) {
            strncpy(epk_archive_dir, epik_dir, PATH_MAX);
            elg_cntl_msg("Archive directory set to %s", epk_archive_dir);
        } else {
            elg_warning("Archive undefined for \"%s\"", name);
            return NULL; /* invalid EPIK directory name */
        }
    } else { /* derive from configuration */
        snprintf(epk_archive_dir, PATH_MAX, "%s/%s%s",
                 epk_get(EPK_GDIR), EPK_DIR_PREFIX, epk_get(EPK_TITLE));
        elg_cntl_msg("Archive derived from environment %s", epk_archive_dir);
    }
    return epk_archive_dir;
}

/* determine whether provided pathname is to an EPIK archive directory */

char* epk_archive_is_epik (const char* path)
{
    int is_epik;
    char* ch, *begch=(char*)path;
    if (!path) return NULL;
    while ( (ch = strstr(begch, "/" EPK_DIR_PREFIX)) != NULL ) begch=ch+1;
    is_epik = ((begch!=path) ||
               (strncmp(path, EPK_DIR_PREFIX, strlen(EPK_DIR_PREFIX))==0));
    if (is_epik) return begch; else return NULL;
}

char* epk_archive_directory (const epk_type_t type)
{
    switch(type) {
        case EPK_TYPE_DIR:
	  return epk_archive_get_name();
        case EPK_TYPE_ESD:
        case EPK_TYPE_ELG:
        case EPK_TYPE_EMP:
        case EPK_TYPE_OTF:
        case EPK_TYPE_CUBE:
            if (epk_archive_dir[0] != '\0') { 
                char* epik_name = (char*) calloc(strlen(epk_archive_get_name())+6, sizeof(char));
                sprintf(epik_name, "%s/%s", epk_archive_get_name(), epk_types[type].name);
                return epik_name;
            } else
	      return strdup(epk_get(EPK_GDIR));
        default:
            return NULL;
    }
}

typedef struct {
    char* enddir; /* pointer to end of name of root directory (with expt) */
    char* begnam; /* pointer to beginning of experiment name */
    char* endnam; /* pointer to end of experiment name */
} splits_t;

   /* strncpy(epik_path, path, splits.endnam-path); */
   /* strncpy(epik_root, path, splits.enddir-path); */
   /* strncpy(epik_name, splits.begnam, splits.endnam-splits.begnam); */
   /* strcpy(epik_rest, splits.endnam); */

int epk_archive_path_tok (const char* path, splits_t* splits)
{
    splits->enddir = epk_archive_is_epik(path);
    if (splits->enddir) {
        splits->begnam = splits->enddir + strlen(EPK_DIR_PREFIX);
        splits->endnam = strchr(splits->begnam,'/');
        if (!splits->endnam) splits->endnam = (char*)path + strlen(path);
    } else {
        splits->begnam=splits->endnam=NULL;
    }
    return (splits->enddir != NULL); /* is_epik or not */
}

char* epk_archive_filename (const epk_type_t type, const char* name)
{
    splits_t splits;
    int is_epik = epk_archive_path_tok(name, &splits);
    char* epik_name = (char*) calloc(strlen(name)+11, sizeof(char));

    switch(type) {
      case EPK_TYPE_DIR:
          if (is_epik) { /* extract directory name from path name */
              strncpy(epik_name, name, splits.endnam-name);
          }
          break;
      case EPK_TYPE_ESD:
      case EPK_TYPE_EMP:
      case EPK_TYPE_ELG:
      case EPK_TYPE_CUBE:
      case EPK_TYPE_SION:
          if (is_epik) sprintf(epik_name, "%s/%s.%s", name, EPK_NAME, epk_types[type].suffix);
          else         sprintf(epik_name, "%s.%s", name, epk_types[type].suffix);
          break;
      default:
          /* EPIK name undefined */
          break;
    }

    return epik_name;
}

char* epk_archive_rankname (const epk_type_t type, const char* name,
                            const unsigned rank)
{
    int is_epik = (epk_archive_is_epik(name) != NULL);
    char* epik_name = (char*) calloc(strlen(name)+12, sizeof(char));

    switch(type) {
      case EPK_TYPE_ESD:
      case EPK_TYPE_ELG:
          if (is_epik) sprintf(epik_name, "%s/%s/%05u", name, epk_types[type].name, rank);
          else         sprintf(epik_name, "%s.%u.%s", name, rank, epk_types[type].suffix);
          break;
      case EPK_TYPE_DIR:
      default:
          /* EPIK name undefined */
          break;
    }

    return epik_name;
}

char* epk_archive_filetype (const char* filename)
{
    int len;
    const char* ext;
    unsigned type=0;
    if (!filename || ((len=strlen(filename))<5))
        return epk_types[EPK_TYPE_MAX].description;
    ext = filename + (len-3);
    do {
        type++;
        if (strstr(filename, epk_types[type].name) ||
                     (strcmp(epk_types[type].suffix, ext)==0))
             break; /* got a match */
    } while (type < EPK_TYPE_MAX);
    return epk_types[type].description;
}

void epk_archive_update (char* filename)
{
    int mark = strlen(filename);
    filename[mark] = '+';
    filename[mark+1] = '\0';
    elg_cntl_msg("Prepared to update archive file %s", filename);
}

void epk_archive_commit (char* filename)
{
    int mark = strlen(filename)-1;
    char tmp_name[PATH_MAX];
    if (filename[mark] != '+') {
        elg_error_msg("Can't commit archive file with name %s", filename);
    }
    strcpy(tmp_name, filename);
    tmp_name[mark] = '\0';
    if (rename(filename, tmp_name) != 0) {
        elg_error_msg("Abort: commit/rename archive file %s", filename);
    } else {
        elg_cntl_msg("Committed archive file %s", filename);
    }
    filename[mark] = '\0';
}

int epk_archive_exists (const char* dir)
{
    struct stat statbuf;
    int is_dir = ((stat(dir ? dir : epk_archive_get_name(), &statbuf) == 0) &&
                  (statbuf.st_mode & S_IFDIR));
    return is_dir;
}

/* verify the non-existance of the specified experiment archive directory */
/* or the default archive directory */

int epk_archive_verify (const char* dir)
{
    struct stat statbuf;
    const char* archive_path = dir ? dir : epk_archive_get_name();
    int blocked = (stat(archive_path, &statbuf) == 0);
    if (blocked) {
        elg_warning("Abort: new measurement blocked by existing %s %s",
                    (statbuf.st_mode & S_IFDIR) ? "directory" : "file",
                    archive_path);
    }
    return !blocked;
}

int epk_archive_locked (void)
{
    char epik_lock[PATH_MAX];
    snprintf(epik_lock, PATH_MAX, "%s/%s", epk_archive_get_name(), "epik.lock");
    int locked = (access(epik_lock, F_OK) == 0);
    elg_cntl_msg("%s=%d", epik_lock, locked);
    return locked;
}

void epk_archive_unlock (void)
{
    char epik_lock[PATH_MAX];
    snprintf(epik_lock, PATH_MAX, "%s/%s", epk_archive_get_name(), "epik.lock");
    if (unlink(epik_lock) != 0)
        elg_warning("Failed to unlink %s: %s", epik_lock, strerror(errno));
    else
        elg_cntl_msg("Unlinked lock file %s", epik_lock);
}

void epk_archive_getcwd (void)
{
    getcwd(epk_archive_cwd, sizeof(epk_archive_cwd)); /* stash starting directory */
    elg_cntl_msg("CWD=%s", epk_archive_cwd);
}

void epk_archive_return (void)
{
    struct stat cwdstatbuf, arcstatbuf;
    char* curdir = getcwd(NULL, PATH_MAX);
    if (epk_archive_cwd[0] == '\0') {
        elg_error_msg("Starting directory was not saved during creation!");
        return;
    }
    if (curdir == NULL) {
        elg_warning("Failed to determine current directory: %s", strerror(errno));
    } else {
        stat(curdir, &cwdstatbuf);
        stat(epk_archive_cwd, &arcstatbuf);
        elg_cntl_msg("inode=%u CWD=%s", cwdstatbuf.st_ino, curdir);
        elg_cntl_msg("inode=%u ARC=%s", arcstatbuf.st_ino, epk_archive_cwd);
    }
    if ((curdir || (cwdstatbuf.st_ino != arcstatbuf.st_ino)) && chdir(epk_archive_cwd)) {
        elg_error_msg("Failed to return to archive directory %s: %s",
                      epk_archive_cwd, strerror(errno));
    }
}

int epk_archive_create (const epk_type_t type)
{
    char dir_path[PATH_MAX];
    mode_t mode = S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH;

    strcpy(dir_path, epk_archive_get_name());
    if (mkdir(dir_path, mode) == 0) { /* successfully created */
        FILE *fc;
        const char* filter_file = epk_get(EPK_FILTER); /* blacklist */
        char epik_file[PATH_MAX];
        elg_cntl_msg("Created measurement archive %s", dir_path);
        snprintf(epik_file, PATH_MAX, "%s/%s", dir_path, "epik.lock");
        if ((fc = fopen(epik_file, "w")) == NULL)
            elg_warning("Failed to create %s", epik_file);
        else {
            if (fclose(fc)) elg_error_msg("Failed to close %s", epik_file);
        }
        snprintf(epik_file, PATH_MAX, "%s/%s", dir_path, "epik.conf");
        if ((fc = fopen(epik_file, "w")) == NULL)
            elg_warning("Failed to open %s for writing", epik_file);
        else {
            epk_conf_print(fc);
            if (fclose(fc)) elg_error_msg("Failed to close %s", epik_file);
        }
        if (filter_file && strlen(filter_file)) {
            /* if filter file is readable, copy it into archive */
            FILE *ff = fopen(filter_file, "r");
            if (ff == NULL)
                elg_cntl_msg("Failed to open \"%s\" for reading", filter_file);
            else {
                snprintf(epik_file, PATH_MAX, "%s/%s", dir_path, "epik.filt");
                if ((fc = fopen(epik_file, "w")) == NULL)
                    elg_warning("Failed to open %s for writing", epik_file);
                else {
                    char buffer[PATH_MAX];
                    size_t bytes_read;
                    while((bytes_read = fread(buffer, sizeof(char), sizeof(buffer), ff)))
                        fwrite(buffer, sizeof(char), bytes_read, fc);
                    if (fclose(fc)) elg_error_msg("Failed to close %s", epik_file);
                }
                if (fclose(ff)) elg_error_msg("Failed to close %s", filter_file);
            }
        }
    } else if (!epk_archive_exists(dir_path)) { /* ignore if already exists */
        elg_error_msg("Abort: failed to create measurement archive %s", dir_path);
        return 0;
    }

    if (type == EPK_TYPE_DIR) return 1;
  
    snprintf(dir_path, PATH_MAX, "%s/%s", epk_archive_get_name(), 
             epk_types[type].name);
    if (mkdir(dir_path, mode) == 0) { /* successfully created */
        elg_cntl_msg("Created new archive subdirectory %s", dir_path);
    } else if (!epk_archive_exists(dir_path)) { /* ignore if already exists */
        elg_error_msg("Abort: failed to create archive subdirectory %s", dir_path);
        return -1;
    }
    return 2;
}

void epk_archive_remove (const epk_type_t type)
{
    char dir_path[PATH_MAX];
    snprintf(dir_path, PATH_MAX, "%s/%s", epk_archive_get_name(), 
             epk_types[type].name);
    if (remove(dir_path) == 0)
        elg_cntl_msg("Removed internal %s directory %s",
                    epk_archive_filetype(dir_path), dir_path);
    else
        elg_warning("Cannot remove internal %s directory %s",
                    epk_archive_filetype(dir_path), dir_path);
}

void epk_archive_remove_file (char* filename)
{
    if (unlink(filename) == 0)
        elg_cntl_msg("Removed %s file %s",
                    epk_archive_filetype(filename), filename);
    else
        elg_warning("Cannot remove %s file %s", 
                    epk_archive_filetype(filename), filename);
}

void epk_archive_remove_all (const epk_type_t type,
                             const char* fprefix, const unsigned ranks)
{
    unsigned rank;
    for (rank = 0; rank < ranks; rank++) {
        char* filename = epk_archive_rankname(type, fprefix, rank);
        epk_archive_remove_file(filename);
        free(filename);
    }
    if (epk_archive_is_epik(fprefix)) {
        epk_archive_set_name(fprefix);
        epk_archive_remove(type);
    }
}

char* epk_get_elgfilename(const char* path)
{
  struct stat statbuf;
  int isDir;

  /* create copy with enough space for addition */
  int pathlen = strlen(path);
  char *efile = malloc(pathlen+10); /* 10 for "/epik.elg" */
  if ( efile == NULL ) elg_error_msg("Can't allocate ELG filename buffer");
  strcpy(efile, path);

  /* trailing '/' ? */
  if ( path[pathlen-1] == '/' ) {
    efile[pathlen-1] = '\0';
    pathlen--;
  }

  /* is directory ? */
  isDir = ((stat(efile, &statbuf) == 0) && (statbuf.st_mode & S_IFDIR));

  if (isDir && epk_archive_is_epik(efile)) {
    /* add ELG default filename */
    strcat(efile, "/epik.elg");
  }
  return efile;
}


