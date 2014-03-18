#include <iostream>
#include <Qt/qdatastream.h>
#include <Qt/qfile.h>
#include <Qt/qiodevice.h>


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

            unsigned int offset = tmpchar2[3] | tmpchar2[0] <<24 | tmpchar2[1]<<16 | tmpchar2[2]<<8  ;   //needs a batter way to figure it out
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
    printf("at position %llu\n", testfile.pos());


    //go through palmdocheader ; 16 bytes
	delete tmpchar;

    tmpchar = new char[16];
    testresult = inStream.readRawData(tmpchar,16);

    if (testresult == 16) {
        unsigned char * tmpchar2 = (unsigned char*)tmpchar;
        for(int j = 0; j < 16; j++)
            printf(" %02x ", tmpchar2[j]);
        printf(" ]\n");

        unsigned int compression = tmpchar2[0] << 8 | tmpchar2[1];
        int dummy = tmpchar2[2] << 8 | tmpchar2[3]; //should be 0, according to spec
        unsigned int textLength = tmpchar2[4] << 24 | tmpchar2[5] << 16 | tmpchar2[6] << 8 | tmpchar2[7] ;
        unsigned int recordCount = tmpchar2[8] << 8 | tmpchar2 [9];
        unsigned int recordSize = tmpchar2[10] << 8 | tmpchar2[11];
        unsigned int encryptionType = tmpchar2[12] << 8 | tmpchar2[13];
        unsigned int unknown = tmpchar2[14] << 8 | tmpchar2[15];

        printf("compression : %0x \n",compression);
        printf("dummy       : %0x \n",dummy);
        printf("textLength  : %0x \n",textLength);
        printf("recCount    : %0x \n",recordCount);
        printf("recSize     : %0x \n",recordSize);
        printf("encType     : %0x \n",encryptionType);
        printf("unk         : %0x \n",unknown);
    }

    printf("parsing record 0\n");

    //go through MOBI header

    //go through EXTH header, if found (indicated in MOBI header)

    return 0;
}




