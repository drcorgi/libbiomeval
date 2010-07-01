/*
 * This software was developed at the National Institute of Standards and
 * Technology (NIST) by employees of the Federal Government in the course
 * of their official duties. Pursuant to title 17 Section 105 of the
 * United States Code, this software is not subject to copyright protection
 * and is in the public domain. NIST assumes no responsibility whatsoever for
 * its use by other parties, and makes no guarantees, expressed or implied,
 * about its quality, reliability, or any other characteristic.
 */

#include <sys/time.h>

#include <iostream>

#include <stdlib.h>

#ifdef FILERECORDSTORETEST
#include <be_filerecstore.h>
using namespace BiometricEvaluation;
#define TESTDEFINED
#endif

#ifdef DBECORDSTORETEST
#include <be_dbrecstore.h>
#define TESTDEFINED
#endif

#ifdef ARCHIVERECORDSTORETEST
#include <be_archiverecstore.h>
#define TESTDEFINED
#endif

#ifdef TESTDEFINED
using namespace BiometricEvaluation;
#endif

#define TIMEINTERVAL(__s, __f)                                          \
	(__f.tv_sec - __s.tv_sec)*1000000+(__f.tv_usec - __s.tv_usec)

const int RECCOUNT = 110503;		/* A prime number of records */
const int RECSIZE = 1153;		/* of prime number size each */
const int KEYNAMESIZE = 32;
static char keyName[KEYNAMESIZE];
static struct timeval starttm, endtm;
static uint64_t totalTime;

/*
 * Insert a suite of records to the RecordStore in order to measure
 * performance in terms of speed and robustness.
 */
static int insertMany(RecordStore *rs)
{
	string *theKey;
	totalTime = 0;
	uint8_t *theData = (uint8_t *)malloc(RECSIZE);
	cout << "Creating " << RECCOUNT << " records of size " << RECSIZE << "." << endl;
	for (int i = 0; i < RECCOUNT; i++) {
		snprintf(keyName, KEYNAMESIZE, "key%u", i);
		theKey = new string(keyName);
		gettimeofday(&starttm, NULL);
		try {
			rs->insert(*theKey, theData, RECSIZE);
		} catch (ObjectExists& e) {
			cout << "Whoops! Record exists?. Insert failed at record " << i << "." << endl;
			return (-1);
		} catch (StrategyError& e) {
			cout << "Could not insert record " << i << ": " <<
			    e.getInfo() << "." << endl;
			return (-1);
		}
		gettimeofday(&endtm, NULL);
		totalTime += TIMEINTERVAL(starttm, endtm);
		delete theKey;
	}
	cout << "Insert lapsed time: " << totalTime << endl;
	return (0);
}

/*
 * Test the read and write operations of a Bitstore, hopefully stressing
 * it enough to gain confidence in its operation. This program should be
 * able to test any implementation of the abstract Bitstore by creating
 * an object of the appropriate implementation class.
 */
int main (int argc, char* argv[]) {

	/*
	 * Other types of Bitstore objects can be created here and
	 * accessed via the Bitstore interface.
	 */

#ifdef FILERECORDSTORETEST

	/* Call the constructor that will create a new FileRecordStore. */
	string rsname("frs_test");
	FileRecordStore *rs;
	try {
		rs = new FileRecordStore(rsname, "RW Test Dir");
	} catch (ObjectExists& e) {
		cout << "The FileRecordStore already exists; exiting." << endl;
		return (EXIT_FAILURE);
	} catch (StrategyError& e) {
		cout << "A strategy error occurred: " << e.getInfo() << endl;
	}
	auto_ptr<FileRecordStore> ars(rs);
#endif

#ifdef DBECORDSTORETEST
	/* Call the constructor that will create a new DBRecordStore. */
	string rsname("dbrs_test");
	DBRecordStore *rs;
	try {
		rs = new DBRecordStore(rsname, "RW Test Dir");
	} catch (ObjectExists& e) {
		cout << "The DBRecordStore already exists; exiting." << endl;
		return (EXIT_FAILURE);
	} catch (StrategyError& e) {
		cout << "A strategy error occurred: " << e.getInfo() << endl;
	}
	auto_ptr<DBRecordStore> ars(rs);
#endif

#ifdef ARCHIVERECORDSTORETEST
	/* Call the constructor that will create a new ArchiveRecordStore. */
	string rsname("ars_test");
	ArchiveRecordStore *rs;
	try {
		rs = new ArchiveRecordStore(rsname, "RW Test Dir");
	} catch (ObjectExists& e) {
		cout << "The ArchiveRecordStore already exists; exiting." << endl;
		return (EXIT_FAILURE);
	} catch (StrategyError& e) {
		cout << "A strategy error occurred: " << e.getInfo() << endl;
	}
	auto_ptr<ArchiveRecordStore> ars(rs);
#endif

#ifdef TESTDEFINED

	/*
	 * From this point forward, all access to the store object, no matter
	 * what subclass, is done via the RecordStore interface.
	 */

	string *theKey;
	uint8_t *theData;

	theData = (uint8_t *)malloc(RECSIZE);
#if 0
	cout << "Creating " << RECCOUNT << " records of size " << RECSIZE << "." << endl;

	/* Insert test */
	totalTime = 0;
	for (int i = 0; i < RECCOUNT; i++) {
		snprintf(keyName, KEYNAMESIZE, "key%u", i);
		theKey = new string(keyName);
		gettimeofday(&starttm, NULL);
		try {
			ars->insert(*theKey, theData, RECSIZE);
		} catch (ObjectExists& e) {
			cout << "Whoops! Record exists?. Insert failed at record " << i << "." << endl;
			return (EXIT_FAILURE);
		} catch (StrategyError& e) {
			cout << "Could not insert record " << i << ": " <<
			    e.getInfo() << "." << endl;
			return (EXIT_FAILURE);
		}
		gettimeofday(&endtm, NULL);
		totalTime += TIMEINTERVAL(starttm, endtm);
		delete theKey;
	}
	cout << "Insert lapsed time: " << totalTime << endl;
#endif
	if (insertMany(rs) != 0)
		return (EXIT_FAILURE);

	/* Random replace test */
	srand(endtm.tv_sec);
	totalTime = 0;
	for (int i = 0; i < RECCOUNT; i++) {
		snprintf(keyName, KEYNAMESIZE, "key%u", 
		    (unsigned int)(rand() % RECCOUNT));
		theKey = new string(keyName);
		gettimeofday(&starttm, NULL);
		try {
			ars->replace(*theKey, theData, RECSIZE);
		} catch (ObjectDoesNotExist& e) {
			cout << "Whoops! Record doesn't exists?. Insert failed at record " << i << "." << endl;
			return (EXIT_FAILURE);
		} catch (StrategyError& e) {
			cout << "Could not replace record " << i << ": " <<
			    e.getInfo() << "." << endl;
			return (EXIT_FAILURE);
		}
		gettimeofday(&endtm, NULL);
		totalTime += TIMEINTERVAL(starttm, endtm);
		delete theKey;
	}
	cout << "Random replace lapsed time: " << totalTime << endl;

	/* Sequential read test */
	totalTime = 0;
	for (int i = 0; i < RECCOUNT; i++) {
		snprintf(keyName, KEYNAMESIZE, "key%u", i);
		theKey = new string(keyName);
		gettimeofday(&starttm, NULL);
		try {
			ars->read(*theKey, theData);
		} catch (ObjectDoesNotExist& e) {
			cout << "Whoops! Record doesn't exist?. Read failed at record " <<
			    i << "." << endl;
			return (EXIT_FAILURE);
		} catch (StrategyError& e) {
			cout << "Could not read record " << i << ": " <<
			    e.getInfo() << "." << endl;
			return (EXIT_FAILURE);
		}
		gettimeofday(&endtm, NULL);
		totalTime += TIMEINTERVAL(starttm, endtm);
		delete theKey;
	}
	cout << "Sequential read lapsed time: " << totalTime << endl;

	/* Random read test */
	totalTime = 0;
	for (int i = 0; i < RECCOUNT; i++) {
		snprintf(keyName, KEYNAMESIZE, "key%u", 
		    (unsigned int)(rand() % RECCOUNT));
		theKey = new string(keyName);
		gettimeofday(&starttm, NULL);
		try {
			ars->read(*theKey, theData);
		} catch (ObjectDoesNotExist& e) {
			cout << "Whoops! Record doesn't exist?. Read failed at record " <<
			    i << "." << endl;
			return (EXIT_FAILURE);
		} catch (StrategyError& e) {
			cout << "Could not read record " << i << ": " <<
			    e.getInfo() << "." << endl;
			return (EXIT_FAILURE);
		}
		gettimeofday(&endtm, NULL);
		totalTime += TIMEINTERVAL(starttm, endtm);
		delete theKey;
	}
	cout << "Random read lapsed time: " << totalTime << endl;

	/* Remove all test */
	uint64_t startStoreSize;
	try {
		startStoreSize = ars->getSpaceUsed();
	} catch (StrategyError& e) {
                cout << "Can't get space usage:" << e.getInfo() << "." << endl;
		return (EXIT_FAILURE);
        }
	cout << "Space used after first insert is " << startStoreSize << endl;

	totalTime = 0;
	for (int i = 0; i < RECCOUNT; i++) {
		snprintf(keyName, KEYNAMESIZE, "key%u", i);
		theKey = new string(keyName);
		gettimeofday(&starttm, NULL);
		try {
			ars->remove(*theKey);
		} catch (ObjectDoesNotExist& e) {
			cout << "Whoops! Record doesn't exist?. Remove failed at record " <<
			    i << "." << endl;
			return (EXIT_FAILURE);
		} catch (StrategyError& e) {
			cout << "Could not remove record " << i << ": " <<
			    e.getInfo() << "." << endl;
			return (EXIT_FAILURE);
		}
		gettimeofday(&endtm, NULL);
		totalTime += TIMEINTERVAL(starttm, endtm);
		delete theKey;
	}
	cout << "Remove lapsed time: " << totalTime << endl;
	ars->sync();
	cout << "Count is now " << ars->getCount() << endl;
	uint64_t endStoreSize;
	try {
		endStoreSize = ars->getSpaceUsed();
	} catch (StrategyError& e) {
                cout << "Can't get space usage:" << e.getInfo() << "." << endl;
		return (EXIT_FAILURE);
        }
	cout << "Space used after removal is " << endStoreSize << endl;

	cout << "Inserting again, after removal... " << endl;
	startStoreSize = endStoreSize;
	if (insertMany(rs) != 0)
		return (EXIT_FAILURE);
	endStoreSize = ars->getSpaceUsed();
	cout << "Space used after second insert is " << endStoreSize << endl;

#endif
	return(EXIT_SUCCESS);
}
