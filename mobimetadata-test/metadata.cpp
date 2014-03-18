#include <iostream>
#include <Qt/qdatastream.h>
#include <Qt/qfile.h>
#include <Qt/qiodevice.h>


//attempt to convert sequence of bytes into integer
unsigned int bytesToInt(char * bytes, int start, int count) {
    unsigned char * tmpchars = (unsigned char *) bytes;
    unsigned int value = 0;
    for (int j=0;j<count;j++) {
        value = (value << 8) + tmpchars[start+j];
    }
    return value;
}

//print hex bytes of a char *
void printbytes(char * input, int count) {
        unsigned char * tmpchar2 = (unsigned char*) input;

        printf(" [ ");
        for(int j = 0; j < count; j++)
            printf(" %02x ", tmpchar2[j]);
        printf(" ]\n");
}


int main()
{
    QFile testfile("test.mobi");

    testfile.open(QIODevice::ReadOnly);
    QDataStream inStream(&testfile);

	/* file starts with pdb header, 
	 * 4bytes at 60 should be 'BOOK'
	 * 4bytes at 64 should be 'MOBI'
	 * if file is a valid .mobi
	 */

    //goto pos 60
    if (!testfile.seek(60)) {
		printf("failed to seek to offset 60\n");
		return 0;
	}

	//read 8 bytes
	char * tmpchar = new  char[8];

    if (inStream.readRawData(tmpchar,8) != 8) {
		printf("failed to read 8 bytes\n");
		return 0;
	}
	//check'em
    if (strncmp(tmpchar,"BOOKMOBI",8) == 0)
        printf ( "got MOBI header\n");

	/* position 76 has number of PDB records
	 * records follow straight after
	 * 
	 */
    //goto pos 76

    testfile.seek(76);

    delete tmpchar;

    tmpchar = new  char[2];

    unsigned int records = 0 ;
    
    if (inStream.readRawData(tmpchar,2) == 2) {
        records = bytesToInt(tmpchar,0,2) ;   //needs a batter way to figure it out
        printf ( "mobi records : %d \n", records);
    }

    printf("parsing record0\n");

    //records start at  78 and take up 8*records bytes of data
    //http://wiki.mobileread.com/wiki/PDB#Palm_Database_Format

    testfile.seek(78);

    delete tmpchar;
    tmpchar = new char[8];

    //4b - record offset from start of file
    //1b - record flags
    //3b - record id

	testfile.seek(testfile.pos() + 8*records);
/*    for (int i=0;i<records;i++) {
        printf("record %d\n", i);
        if (inStream.readRawData(tmpchar,8) != 8) {
			printf("file ended prematurely. exiting\n");
			return 0;
		}

		//printbytes(tmpchar,8);

		unsigned int offset = bytesToInt(tmpchar,0,4)  ;   //needs a batter way to figure it out
		int flags = tmpchar2[4];
		unsigned int record_id = bytesToInt(tmpchar,5,2);
            
		printf ( " -- offset : %0x [%d] \n", offset, offset);
		printf ( " -- flags : %0x [%d] \n", flags, flags);
		printf ( " -- record_id : %0x [%d] \n", record_id, record_id);
       
    } 
*/

/* there are 2 bytes of padding after records, skip them */

	testfile.seek(testfile.pos() + 2);

    /* 16 byte palmdoc header starts here, read it */
	delete tmpchar;

	//save this location as reference for later
    qint64 header0pos = testfile.pos();

	
/*	tmpchar = new char[16];
	testresult = inStream.readRawData(tmpchar,16);

    if (testresult == 16) {

		printbytes(tmpchar,16);

        //we don't really need this, just for verification
        printf("compression : %0x \n",bytesToInt(tmpchar,0,2));
        printf("dummy       : %0x (expected 0)\n",bytesToInt(tmpchar,2,2));
        printf("textLength  : %0x \n",bytesToInt(tmpchar,4,4));
        printf("recCount    : %0x \n",bytesToInt(tmpchar,8,2));
        printf("recSize     : %0x (expected 0x1000)\n",bytesToInt(tmpchar,10,2));
        printf("encType     : %0x \n",bytesToInt(tmpchar,12,2));
        printf("unk         : %0x \n",bytesToInt(tmpchar,14,2));
    }
*/


	/* instead of parsing, we just go over it and skip to record #0 that follows */
	testfile.seek(testfile.pos() + 16);

    //go through MOBI header
    //first let's see if we have a header, 4 bytes should be 'MOBI', 16 bytes from beginning of PalmDoc header

	tmpchar = new char[4];
    
    if ((inStream.readRawData(tmpchar,4) == 4) && (strncmp(tmpchar,"MOBI",4) == 0))
        printf("got MOBI header in record 0\n");

    //next up is header length that includes that 'MOBI' signature just read.
    unsigned int mobiHeaderSize  = 0;

    if (inStream.readRawData(tmpchar,4) == 4) {
        mobiHeaderSize = bytesToInt(tmpchar,0,4);
    }

    printf("mobi header size : %d [ %0x ]\n", mobiHeaderSize, mobiHeaderSize);

    printf("EXTH should start at %llx\n", mobiHeaderSize + header0pos + 0x10);  //add 16 bytes for palmdoc header parsed previously

    //check if EXTH record exists

    testfile.seek(header0pos + 0x80) ; //location of EXTH flags in header;
    bool got_exth_header = false;

    if (inStream.readRawData(tmpchar,4) == 4)
    {
//		printbytes(tmpchar,4);

        unsigned int exth_flags = bytesToInt(tmpchar,0,4);

        if ( exth_flags & 0x40)
            got_exth_header = true;
    }

    if (!got_exth_header) {
        printf("EXTH not found, exiting\n");
        return 0;
    }

    printf("EXTH header exists\n");

    //if EXTH header size is not divisible by 4, it has to be padded to a multiple of 4, and then actual data starts.
    int exth_padding = 0;

    if (mobiHeaderSize % 4 )
        exth_padding = 4 - mobiHeaderSize % 4 ;

    printf(" exth padding : %d\n", exth_padding);


    //go through EXTH header, if found (indicated in MOBI header)

    //navigating to start of EXTH
    printf("seeking to exth header\n");
    qint64 exth_pos = header0pos + mobiHeaderSize+0x10;  //first palmdoc entry offset + 16bytes + entire mobi header is where exth starts
    testfile.seek(exth_pos);

    printf("at position %llu [ %llx ]\n", testfile.pos(), testfile.pos());
    /*
    'EXTH'
    4 bytes - length of entire header
    4 bytes - number of records

    < records follow >
    record {
        4 bytes      type
        4 bytes      length of entire record (that is, including 8 bytes for type and length)
        length-8     actual record data
    }

    0-3 bytes padding (in exth_padding)
    */

    delete tmpchar;
    tmpchar = new char [12];


    printf("reading 12 bytes\n");
    if (inStream.readRawData(tmpchar,12) == 12) {
//        printbytes(tmpchar,12);
        if (strncpy(tmpchar,"EXTH",4) == 0)
            printf("got EXTH header\n");
        unsigned int headerlength = bytesToInt(tmpchar,4,4);
        unsigned int exthRecords = bytesToInt(tmpchar,8,4);

        printf("header is %x long \n", headerlength);
        printf("there are %x records \n",exthRecords);

        //go through the EXTH records
        for (unsigned int j=0;j<exthRecords;j++) {
            char * tmprecord = new char [8];
            if (inStream.readRawData(tmprecord,8) ==8 ) {
//                printf("record %d/%d\n",j,exthRecords);
                unsigned int recType = bytesToInt(tmprecord,0,4);
//                printf(" type   : %d\n",recType);
                unsigned int recLength = bytesToInt(tmprecord,4,4);
//                printf(" length : %0x\n", recLength);

                char * recordData = new char[(int)recLength - 8];
                if (inStream.readRawData(recordData,(int)recLength - 8) == (int)recLength -8) {
//                    printf(" data : %s\n", recordData);
//					printbytes(recordData,(int)recLength -8);
                }

                switch (recType) {
                case 100:
                    printf("author : %s \n", recordData);
                    break;
                case 106:
                    printf("date : %s \n", recordData);
                    break;
				case 105:
                    printf("genre : %s \n", recordData);
                    break;
                case 503:
                    printf("title : %s \n", recordData);
                    break;
                case 524:
                    printf("lang : %s \n", recordData);
                    break;
                    
                default:
                    break;
                }
            }
            delete tmprecord;
        }
    }
    return 0;
}




