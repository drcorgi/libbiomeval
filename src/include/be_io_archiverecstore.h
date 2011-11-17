/*
 * This software was developed at the National Institute of Standards and
 * Technology (NIST) by employees of the Federal Government in the course
 * of their official duties. Pursuant to title 17 Section 105 of the
 * United States Code, this software is not subject to copyright protection
 * and is in the public domain. NIST assumes no responsibility whatsoever for
 * its use by other parties, and makes no guarantees, expressed or implied,
 * about its quality, reliability, or any other characteristic.
 */

#ifndef __BE_ARCHIVERECSTORE_H__
#define __BE_ARCHIVERECSTORE_H__

#include <exception>
#include <map>
#include <string>

#include <be_io_recordstore.h>

#define ARCHIVE_RECORD_REMOVED	-1
#ifndef MAXLINELEN
#define MAXLINELEN 255
#endif /* MAXLINELEN */

using namespace std;

/** Name of the manifest file on disk */
const string manifestFileName("manifest");
/** Name of the archive file on disk */
const string archiveFileName("archive");

namespace BiometricEvaluation {

	namespace IO {
		/** Info about a single archive element */
		typedef struct {
			/** The offset from the beginning of the file/memory */
			long offset;
			/** The length from offset this element spans */
			uint64_t size;
		} ManifestEntry;
	
		/** Convenience typedef for storing the manifest */
		typedef map<string, ManifestEntry> ManifestMap;
	
/**
 * @brief
 * This class implements the IO::RecordStore interface by storing data items
 * in single file, with an associated manifest file. 
 * 
 * @details
 * Archives consist of binary records written back to back of each other. 
 * To pull information out of an archive, a manifest file is written in the 
 * same directory as the archive file.
 *
 * Each record is assigned a string key, which will be required for 
 * retrieving the data.  As the data is written, a plain text entry is 
 * entered into the manifest in the format:
 * \n
 * 	key offset size
 * \n
 * where offset is the offset into the archive file key's data chunk resides
 * and size is the length of key's data chunk.
 *
 * By default, information is not removed when updated in the archive, rather
 * the old information is ignored.  Therefore, it is possible to have multiple
 * entries in the manifest for one key.  The last entry for the key is 
 * considered accurate.  If the last offset for a key is 
 * ARCHIVE_RECORD_REMOVED, the information is treated as unavailable.
 */
		class ArchiveRecordStore : public RecordStore {
		public:	
			/**
			 * Create a new ArchiveRecordStore, read/write mode.
			 *
			 * @param[in] name
			 *	The name of the store.
			 * @param[in] description
			 *	The store's description.
			 * @param[in] parentDir
			 * 	The directory where the store is to be created.
			 *
			 * @throw Error::ObjectExists
			 * 	The store already exists.
			 * @throw Error::StrategyError
			 * 	An error occurred when accessing the underlying
			 * 	file system.
			 */
			ArchiveRecordStore(
			    const string &name,
			    const string &description,
			    const string &parentDir)
			    throw (Error::ObjectExists, Error::StrategyError);
	
			/**
			 * Open an existing ArchiveRecordStore.
			 *
			 * @param[in] name
			 *	The name of the store.
			 * @param[in] parentDir
			 * 	The directory where the store is to be created.
			 * @param[in] mode
			 *	Open mode, read-only or read-write.
			 *
			 * @throw Error::ObjectDoesNotExist
			 *	The store does not exist.
			 * @throw Error::StrategyError
			 *	An error occurred when accessing the underlying
			 *	file system.
			 */
			 ArchiveRecordStore(
			     const string &name,
			     const string &parentDir,
			     uint8_t mode = IO::READWRITE)
			     throw (Error::ObjectDoesNotExist, 
			     Error::StrategyError);

			/**
			 * Destructor.
			 */
			~ArchiveRecordStore();
			
			uint64_t getSpaceUsed() 
			    throw (Error::StrategyError);

			/*
			 * Implementations of RecordStore methods.
			 */
			void sync()
			    throw (Error::StrategyError);

			void insert(
			    const string &key,
			    const void *const data,
			    const uint64_t size)
			    throw (Error::ObjectExists, Error::StrategyError);

			void remove(
			    const string &key)
			    throw (Error::ObjectDoesNotExist, 
			    Error::StrategyError);

			uint64_t read(
			    const string &key,
			    void *const data)
			    throw (Error::ObjectDoesNotExist, 
			    Error::StrategyError);

			void replace(
			    const string &key,
			    const void *const data,
			    const uint64_t size)
			    throw (Error::ObjectDoesNotExist, 
			    Error::StrategyError);
	
			uint64_t length(
			    const string &key) 
			    throw (Error::ObjectDoesNotExist);

			void flush(
			    const string &key)
			    throw (Error::ObjectDoesNotExist, 
			    Error::StrategyError);
			
			uint64_t sequence(
			    string &key,
			    void *const data = NULL,
			    int cursor = BE_RECSTORE_SEQ_NEXT)
			    throw (Error::ObjectDoesNotExist, 
			    Error::StrategyError);

			void setCursorAtKey(
			    string &key)
			    throw (Error::ObjectDoesNotExist,
			    Error::StrategyError);
	
			void changeName(
			    const string &name)
			    throw (Error::ObjectExists, Error::StrategyError);
	
			/**
			 * See if the ArchiveRecordStore would benefit from
			 * calling vacuum() to remove deleted entries, since
			 * vacuum() is an expensive operation.
			 *
			 * @return
			 *	true if vacuum() would be beneficial
			 *	false otherwise
			 */
			bool needsVacuum();

			/**
			 * See if the ArchiveRecordStore would benefit from
			 * calling vacuum() to remove deleted entries, since
			 * vacuum() is an expensive operation.
			 *
			 * @param[in] name
			 *	The name of the existing RecordStore.
			 * @param[in] parentDir
			 *	Where, in the filesystem, the store is rooted.
			 * @throw Error::ObjectDoesNotExist
			 *	A record with the given key does not exist.
			 * @throw Error::StrategyError
			 *	An error occurred when using the underlying
			 *	storage system.
			 * @return
			 *	true if vacuum() would be beneficial
			 *	false otherwise
			 */
			static bool needsVacuum(
			    const string &name, 
    			    const string &parentDir)
    			    throw (Error::ObjectDoesNotExist,
			    Error::StrategyError);

			/**
			 * Remove deleted entries from the manifest and 
			 * archive files to save space on disk.
			 *
			 * @param[in] name
			 *	The name of the existing RecordStore.
			 * @param[in] parentDir
			 *	Where, in the file system, the store is rooted.
			 * @throw Error::ObjectDoesNotExist
			 *	A record with the given key does not exist.
			 * @throw Error::StrategyError
			 *	An error occurred when using the underlying
			 *	storage system.
			 * @note
			 * This is an expensive operation.
			 */
			static void vacuum(
			    const string &name,
			    const string &parentDir)
			    throw (Error::ObjectDoesNotExist, 
			    Error::StrategyError);
	
			/**
			 * Obtain the name of the file storing the data for 
			 * this store.
			 *
			 * @return
			 *	Path to archive file.
			 */
			string getArchiveName() const;
	
			/**
			 * Obtain the name of the file storing the manifest data
			 * data for this store.
			 *
			 * @return
			 *	Path to manifest file.
			 */
			string getManifestName() const;
	
		protected:
		
		private:
			/* Prevent copying of ArchiveRecordStore objects */
			ArchiveRecordStore(const ArchiveRecordStore&);
			ArchiveRecordStore& operator=(const ArchiveRecordStore&);

			/** Manifest file handle */
			FILE *_manifestfp;
			/** Archive file handle */
			FILE *_archivefp;
	
			/*
			 * Offsets and sizes of data chunks within the archive.
			 */
			ManifestMap _entries;
	
			/** Position of iterator (for sequence()) */
			ManifestMap::const_iterator _cursorPos;

			/**
			 * Whether or not the ArchiveRecordStore contains a 
			 * deleted entry and would benefit from vacuum().
			 */
			bool _dirty;
			
			/**
			 * Read the manifest.
			 */
			void read_manifest() throw (Error::FileError);
		
			/**
			 * @brief
			 * Write to the manifest.
			 *
			 * @param[in] key
			 *	A unique key for the data chunk
			 * @param[in] record
			 *	Information about key, populated by caller
			 *
			 * @return
			 * 	0 on success, -1 on failure
			 *
			 * @throw Error::StrategyError
			 *	Problem with storage system
			 */
			void
			write_manifest_entry(
			    const string &key, 
			    ManifestEntry entry)
			    throw (Error::StrategyError);
	
			/**
			 * @brief
			 * Open the manifest and archive file streams
			 *
			 * @throw Error::FileError
			 *	Unable to open streams
			 */
			void
			open_streams()
			    throw (Error::FileError);
	
			/**
			 * @brief
			 * Close the manifest and archive file streams
			 *
			 * @throw Error::StrategyError
			 *	Unable to close streams
			 */
			void
			close_streams()
			    throw (Error::StrategyError);
	
			/**
			 * @brief
			 * Use the most efficient method for inserting an item
			 * into a map.
			 *
			 * @param[in] m
			 *	Map to modify
			 * @param[in] k
			 *	The key value
			 * @param[in] v
			 *	The value relating to the key
			 *
			 * @return
			 *	Iterator to the object you inserted/updated
			 *
			 * @note
			 * Based on Scott Meyers's suggestions from Item 24 in
			 * "Effective STL."
			 */
			ManifestMap::iterator
			efficient_insert(
			    ManifestMap &m,
			    const ManifestMap::key_type &k,
			    const ManifestMap::mapped_type &v);
	
			/**
			 * @brief
			 * Check to see if a key exists in the manifest's map
			 *
			 * @param[in] m
			 *	Map to check
			 * @param[in] k
			 *	The key to look for
			 *
			 * @return
			 *	true if key exists, otherwise false
			 */
			bool
			key_exists(
			    ManifestMap &m,
			    const ManifestMap::key_type &k);
		};
	}
}

#endif /* __BE_ARCHIVERECSTORE_H__ */

