/*
 * This software was developed at the National Institute of Standards and
 * Technology (NIST) by employees of the Federal Government in the course
 * of their official duties. Pursuant to title 17 Section 105 of the
 * United States Code, this software is not subject to copyright protection
 * and is in the public domain. NIST assumes no responsibility whatsoever for
 * its use by other parties, and makes no guarantees, expressed or implied,
 * about its quality, reliability, or any other characteristic.
 */

#ifndef __BE_IO_COMPRESSOR__
#define __BE_IO_COMPRESSOR__

#include <map>
#include <string>
#include <tr1/memory>

#include <be_error_exception.h>
#include <be_io_properties.h>
#include <be_memory_autoarray.h>

using namespace std;

namespace BiometricEvaluation 
{
	namespace IO
	{
		/** Implementations for compressing and decompressing data */
		class Compressor
		{
		public:
			/** Kinds of Compressors (for factory) */
			typedef enum {
				GZIP
			} Kind;
			
			/** String representations of the compressors */
			static const string GZIPTYPE;
			
			/**
			 * @brief
			 * Convert Kind enumeration to string.
			 *
			 * @param[in] compressor
			 *	The Compressor to convert.
			 *
			 * @return
			 *	String representation of compressor.
			 *
			 * @throw Error::ObjectDoesNotExist
			 *	compressor is not a valid Compressor type.
			 */
			static string
			kindToString(
			    Compressor::Kind compressor)
			    throw (Error::ObjectDoesNotExist);
			
			/**
			 * @brief
			 * Convert string to Kind enumeration.
			 *
			 * @param[in] compressor
			 *	The Compressor to convert.
			 *
			 * @return
			 *	Kind enumeration of compressor.
			 *
			 * @throw Error::ObjectDoesNotExist
			 *	compressor is not a valid Compressor type.
			 */
			static Compressor::Kind
			stringToKind(
			    const string &compressor)
			    throw (Error::ObjectDoesNotExist);
		
			/**
			 * @brief
			 * Create a new Compressor object.
			 * @details
			 * Default compression options will be used.
			 */
			Compressor();
			
			/**
			 * @brief
			 * Compress a buffer.
			 *
			 * @param uncompressedData
			 *	Uncompressed data buffer to compress.
			 * @param uncompressedDataSize
			 *	Size of uncompressedData.
			 *
			 * @return
			 *	Compressed buffer.
			 *
			 * @throw Error::StrategyError
			 *	Error in compression unit.
			 */
			virtual Memory::uint8Array
			compress(
			    const uint8_t *const uncompressedData,
			    uint64_t uncompressedDataSize)
			    const
			    throw (Error::StrategyError) = 0;
			
			/**
			 * @brief
			 * Compress a buffer.
			 *
			 * @param uncompressedData
			 *	Uncompressed data buffer to compress.
			 *
			 * @return
			 *	Compressed buffer.
			 *
			 * @throw Error::StrategyError
			 *	Error in decompression unit.
			 */
			virtual Memory::uint8Array
			compress(
			    const Memory::uint8Array &uncompressedData)
			    const
			    throw (Error::StrategyError) = 0;
			    
			/**
			 * @brief
			 * Compress a buffer.
			 *
			 * @param uncompressedData
			 *	Uncompressed data buffer to compress.
			 * @param uncompressedDataSize
			 *	Size of uncompressedData.
			 * @param outputFile
			 *	Location to save compressed file.
			 *
			 * @throw Error::ObjectExists
			 *	Output file already exists.
			 * @throw Error::StrategyError
			 *	Error in compression unit.
			 */
			virtual void
			compress(
			    const uint8_t *const uncompressedData,
			    uint64_t uncompressedDataSize,
			    const string &outputFile)
			    const
			    throw (Error::ObjectExists,
			    Error::StrategyError) = 0;
			
			/**
			 * @brief
			 * Compress a buffer.
			 *
			 * @param uncompressedData
			 *	Uncompressed data buffer to compress.
			 * @param outputFile
			 *	Location to save compressed file.
			 *
			 * @throw Error::ObjectExists
			 *	Output file already exists.
			 * @throw Error::StrategyError
			 *	Error in decompression unit.
			 */
			virtual void
			compress(
			    const Memory::uint8Array &uncompressedData,
			    const string &outputFile)
			    const
			    throw (Error::ObjectExists,
			    Error::StrategyError) = 0;
			    
			/**
			 * @brief
			 * Compress a file.
			 *
			 * @param inputFile
			 *	Path to file to compress.
			 *
			 * @return
			 *	Compressed buffer.
			 *
			 * @throw Error::ObjectDoesNotExist
			 *	Input file does not exist.
			 * @throw Error::StrategyError
			 *	Error in decompression unit.
			 */
			virtual Memory::uint8Array
			compress(
			    const string &inputFile)
			    const
			    throw (Error::ObjectDoesNotExist,
			    Error::StrategyError) = 0;
			
			/**
			 * @brief
			 * Compress a file.
			 *
			 * @param inputFile
			 *	Path to file to compress.
			 * @param outputFile
			 *	Path to location where compressed version
			 *	will be saved.
			 *
			 * @throw Error::ObjectDoesNotExist
			 *	Input file does not exist.
			 * @throw Error::ObjectExists
			 *	Output file already exists.
			 * @throw Error::StrategyError
			 *	Error in decompression unit.
			 */
			virtual void
			compress(
			    const string &inputFile,
			    const string &outputFile)
			    const
			    throw (Error::ObjectDoesNotExist,
			    Error::ObjectExists,
			    Error::StrategyError) = 0;
			
			/**
			 * @brief
			 * Decompress a compressed buffer.
			 *
			 * @param compressedData
			 *	Compressed data buffer to decompress.
			 * @param compressedDataSize
			 *	Size of compressedData.
			 * @return
			 *	Decompressed buffer.
			 *
			 * @throw Error::StrategyError
			 *	Error in compression unit.
			 */
			virtual Memory::uint8Array
			decompress(
			    const uint8_t *const compressedData,
			    uint64_t compressedDataSize)
			    const
			    throw (Error::StrategyError) = 0;

			/**
			 * @brief
			 * Decompress a compressed buffer.
			 *
			 * @param compressedData
			 *	Compressed data buffer to decompress.
			 *
			 * @return
			 *	Decompressed buffer.
			 *
			 * @throw Error::StrategyError
			 *	Error in decompression unit.
			 */
   			virtual Memory::uint8Array
			decompress(
			    const Memory::uint8Array &compressedData)
			    const
			    throw (Error::StrategyError) = 0;
			
			/**
			 * @brief
			 * Decompress a compressed buffer into a file.
			 *
			 * @param inputFile
			 *	Location to save compressed file.
			 *
			 * @return
			 *	Decompressed buffer.
			 *
			 * @throw Error::StrategyError
			 *	Error in decompression unit.
			 * @throw Error::ObjectDoesNotExists
			 *	Output file already exists.
			 */
   			virtual Memory::uint8Array
			decompress(
			    const string &inputFile)
			    const
			    throw (Error::ObjectDoesNotExist,
			    Error::StrategyError) = 0;
			    
			/**
			 * @brief
			 * Decompress a file.
			 *
			 * @param compressedData
			 *	Compressed data buffer to decompress.
			 * @param outputFile
			 *	Path to location where decompressed version
			 *	will be saved.
			 *
			 * @throw Error::ObjectExists
			 *	Output file already exists.
			 * @throw Error::StrategyError
			 *	Error in compression unit.
			 */
			virtual void
			decompress(
			    const Memory::uint8Array &compressedData,
			    const string &outputFile)
			    const
			    throw (Error::ObjectExists,
			    Error::StrategyError) = 0;
			    
			/**
			 * @brief
			 * Decompress a file.
			 *
			 * @param compressedData
			 *	Compressed data buffer to decompress.
 			 * @param compressedDataSize
			 *	Size of compressedData.
			 * @param outputFile
			 *	Path to location where decompressed version
			 *	will be saved.
			 *
			 * @throw Error::ObjectExists
			 *	Output file already exists.
			 * @throw Error::StrategyError
			 *	Error in compression unit.
			 */
			virtual void
			decompress(
			    const uint8_t *const compressedData,
			    const uint64_t compressedDataSize,
			    const string &outputFile)
			    const
			    throw (Error::ObjectExists,
			    Error::StrategyError) = 0;
			
			/**
			 * @brief
			 * Decompress a file.
			 *
			 * @param inputFile
			 *	Path to file to decompress.
			 * @param outputFile
			 *	Path to location where decompressed version
			 *	will be saved.
			 *
			 * @throw Error::ObjectDoesNotExist
			 *	Input file does not exist.
			 * @throw Error::ObjectExists
			 *	Output file already exists.
			 * @throw Error::StrategyError
			 *	Error in compression unit.
			 */
			virtual void
			decompress(
			    const string &inputFile,
			    const string &outputFile)
			    const
			    throw (Error::ObjectDoesNotExist,
			    Error::ObjectExists,
			    Error::StrategyError) = 0;
			
			/**
			 * @brief
			 * Assign a compressor option.
			 * @details
			 * Will overwrite existing values without warning.
			 *
			 * @param optionName
			 *	Name of the option to add.
			 * @param optionValue
			 *	Value of the option.
			 *
			 * @throw Error::StrategyError
			 *	Error setting option.
			 */
			void
			setOption(
			    const string &optionName,
			    const string &optionValue)
			    throw (Error::StrategyError);
			    
			/**
			 * @brief
			 * Assign a compressor option.
			 * @details
			 * Will overwrite existing values without warning.
			 *
			 * @param optionName
			 *	Name of the option to add.
			 * @param optionValue
			 *	Value of the option.
 			 *
			 * @throw Error::StrategyError
			 *	Error setting option.
			 */
			void
			setOption(
			    const string &optionName,
			    int64_t optionValue)
			    throw (Error::StrategyError);
			    
			/**
			 * @brief
			 * Obtain a compressor option as an integer.
			 *
			 * @param optionName
			 *	Name of the option to obtain.
			 *
			 * @return
			 *	Value of compressor option.
			 */
			string
			getOption(
			    const string &optionName)
			    const
			    throw (Error::ObjectDoesNotExist);
			    
			/**
			 * @brief
			 * Obtain a compressor option as an integer.
			 *
			 * @param optionName
			 *	Name of the option to obtain.
			 *
			 * @return
			 *	Value of compressor option.
			 *
			 * @throw Error::ObjectDoesNotExist
			 *	The option was never set.
			 */
			int64_t
			getOptionAsInteger(
			    const string &optionName)
			    const
			    throw (Error::ObjectDoesNotExist);
			    
			/**
			 * @brief
			 * Remove a compressor option.
			 *
			 * @param optionName
			 *	Name of the option to remove.
			 */
			void
			removeOption(
			    const string &optionName)
			    throw (Error::ObjectDoesNotExist);
			    
			/** Destructor */
			virtual ~Compressor();
			
			/**
			 * Compressor factory.
			 *
			 * @param compressorKind
			 *	A known kind of compressor.
			 * 
			 * @return
			 *	A new compressor with default options.
			 *
			 * @throw Error::ObjectDoesNotExist
			 *	Invalid compressor type.
			 */
			static tr1::shared_ptr<Compressor>
			createCompressor(
			    Compressor::Kind compressorKind = Compressor::GZIP)
			    throw (Error::ObjectDoesNotExist);
			    
		private:
			/**
			 * @brief
			 * Copy constructor (disabled).
			 * @details
			 * Disabled because Properties member cannot be copied.
			 *
			 * @param other
			 *	Compressor to copy.
			 */
			Compressor(
			    const Compressor &other);
			    
    			/**
			 * @brief
			 * Assignment overload (disabled).
			 * @details
			 * Disabled because Properties member cannot be
			 * assigned.
			 *
			 * @param other
			 *	Compressor to assign.
			 *
			 * @return
			 *	lhs Compressor.
			 */
			Compressor&
			operator=(
			    const Compressor& other);
			    
			/** Compressor properties */
			Properties _compressorOptions;
		};
	}
}

#endif /* __BE_IO_COMPRESSOR__ */
