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

    int testresult = 0;

    //goto pos 60
    testresult = inStream.skipRawData ( 60 ) ;

    if (testresult > 0 )
		printf( "skipped %d bytes\n", testresult);

     char * tmpchar = new  char[8];

    //read 8 bytes
    testresult = inStream.readRawData(tmpchar,8);

    if (testresult > 0 )
		printf( "skipped %d bytes\n", testresult);

    printf( "tmpchar : %s\n", tmpchar);

    if (strncmp(tmpchar,"BOOKMOBI",8) == 0)
        printf ( "got MOBI header\n");

    //goto pos 76

    testfile.seek(76);
    printf("at position %llu\n", testfile.pos());

    delete tmpchar;

    tmpchar = new  char[2];

    int records = 0 ;
    testresult = inStream.readRawData(tmpchar,2);
    if (testresult == 2) {
        records = tmpchar[0] <<8 | tmpchar[1] ;   //needs a batter way to figure it out
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

    for (int i=0;i<records;i++) {
        printf("record %d\n", i);
        testresult = inStream.readRawData(tmpchar,8);
        unsigned char * tmpchar2 = (unsigned char*)tmpchar;
        if (testresult == 8) {

			printf(" [ ");
            for(int j = 0; j < 8; j++)
                printf(" %02x ", tmpchar2[j]);
            printf(" ]\n");

            unsigned int offset = bytesToInt(tmpchar,0,4)  ;   //needs a batter way to figure it out
            int flags = tmpchar2[4];
            unsigned int record_id = tmpchar2[5] << 16 | tmpchar2[6] << 8 | tmpchar2[7];
            printf ( " -- offset : %0x [%d] \n", offset, offset);
            printf ( " -- flags : %0x [%d] \n", flags, flags);
            printf ( " -- record_id : %0x [%d] \n", record_id, record_id);
        }
    }

    printf("at position %lld\n", testfile.pos());

    printf("jumping the 2 byte gap to actual data\n");
    testfile.seek(testfile.pos() + 2);
    printf("at position %llu [ %llx ]\n", testfile.pos(), testfile.pos());


    //go through palmdocheader ; 16 bytes
	delete tmpchar;

	//save this location as reference for later
    qint64 header0pos = testfile.pos();

    tmpchar = new char[16];
    testresult = inStream.readRawData(tmpchar,16);

    if (testresult == 16) {
        unsigned char * tmpchar2 = (unsigned char*)tmpchar;
        printf(" [ ");
        for(int j = 0; j < 16; j++)
            printf(" %02x ", tmpchar2[j]);
        printf(" ]\n");

        //we don't really need this, just for verification
        printf("compression : %0x \n",bytesToInt(tmpchar,0,2));
        printf("dummy       : %0x (expected 0)\n",bytesToInt(tmpchar,2,2));
        printf("textLength  : %0x \n",bytesToInt(tmpchar,4,4));
        printf("recCount    : %0x \n",bytesToInt(tmpchar,8,2));
        printf("recSize     : %0x (expected 0x1000)\n",bytesToInt(tmpchar,10,2));
        printf("encType     : %0x \n",bytesToInt(tmpchar,12,2));
        printf("unk         : %0x \n",bytesToInt(tmpchar,14,2));
    }

    //go through MOBI header

    printf("parsing record 0\n");

    //let's save the starting point, it'll be useful next

    printf("at position %llu [ %llx ]\n", testfile.pos(), testfile.pos());
    //first let's see if we have a header, 4 bytes should be 'MOBI', 16 bytes from beginning for PalmDoc header

    delete tmpchar;
    tmpchar = new char[4];
    testresult = inStream.readRawData(tmpchar,4);

    if ((testresult == 4) && (strncmp(tmpchar,"MOBI",4) == 0))
        printf("got MOBI header in record 0\n");

    //next up is header length that includes that 'MOBI' signature just read.
    testresult = inStream.readRawData(tmpchar,4);
    unsigned int mobiHeaderSize  = 0;

    if (testresult == 4) {
        mobiHeaderSize = bytesToInt(tmpchar,0,4);
    }

    printf("mobi header size : %d [ %0x ]\n", mobiHeaderSize, mobiHeaderSize);

    printf("EXTH should start at %llx\n", mobiHeaderSize + header0pos + 0x10);  //add 16 bytes for palmdoc header parsed previously

    //check if EXTH record exists

    testfile.seek(header0pos + 0x80) ; //location of EXTH flags in header;
    bool got_exth_header = false;

    if (inStream.readRawData(tmpchar,4) == 4)
    {
        unsigned char * tmpchar2 = (unsigned char*) tmpchar;

        printf(" EXTH flags : [ ");
        for(int j = 0; j < 4; j++)
            printf(" %02x ", tmpchar2[j]);
        printf(" ]\n");

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
        8 - length   actual record data
    }

    0-3 bytes padding (in exth_padding)
    */

    delete tmpchar;
    tmpchar = new char [12];


    printf("reading 12 bytes\n");
    if (inStream.readRawData(tmpchar,12) == 12) {
        printbytes(tmpchar,12);
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
                printf("record %d/%d\n",j,exthRecords);
                unsigned int recType = bytesToInt(tmprecord,0,4);
                printf(" type   : %d\n",recType);
                unsigned int recLength = bytesToInt(tmprecord,4,4);
                printf(" length : %0x\n", recLength);

                char * recordData = new char[(int)recLength - 8];
                if (inStream.readRawData(recordData,(int)recLength - 8) == (int)recLength -8) {
                    printf(" data : %s\n", recordData);
                }

                switch (recType) {
                case 100:
                    printf("author : %s \n", recordData);
                    break;
                case 503:
                    printf("title : %s \n", recordData);
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




