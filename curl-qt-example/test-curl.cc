#include <curl/curl.h>
#include <Qt/qstring.h>
#include <Qt/qxmlstream.h>
#include <iostream>

QString data;

size_t readData( char *ptr, size_t size, size_t nmemb, void *userdata) {

	//haven't yet figured out how to pass to *userdata without losing the pointer. ho hum.
	//this is a viable workaround

	data = QString( static_cast<char*>( ptr ) );

	return size*nmemb;
};

void parseLink(QXmlStreamReader &xml) { 
	Q_ASSERT(xml.name() == "link");

	QXmlStreamAttributes attributes = xml.attributes();

	QString url  = attributes.value("href").toString();
	QString rel  = attributes.value("rel").toString();
	QString type = attributes.value("type").toString();

	if (!url.isEmpty()) {
		if (!rel.isEmpty()) {
			if (rel == "http://opds-spec.org/cover") 
				printf("cover : ");
			if (rel == "http://opds-spec.org/thumbnail") 
				printf("thumb : ");
			if (rel == "http://opds-spec.org/acquisition") 
				printf("get   : ");
			if (rel == "last")
				printf(" <<   : ");
			if (rel == "next")
				printf(" >>   : ");
			if (rel == "first")
				printf("first : ");
			if (rel == "previous")
				printf("prev  : ");
			if (rel == "start")
				printf("start : ");
			if (rel == "up")
				printf(" ^^   : ");

		}
		if (rel != "search" ) {
			std::cout
				<< url.toStdString() << std::endl;
			if (!type.isEmpty()) 
				std::cout << "type  : " << type.toStdString() << std::endl;
		}
	}
}


void readEntry(QXmlStreamReader &xml) {
	Q_ASSERT(xml.isStartElement() && xml.name() == "entry");
	printf(" -- entry -- \n");

	QXmlStreamReader::TokenType tmptoken;

	while ((tmptoken = xml.readNext()) && xml.name() != "entry") {
		QXmlStreamAttributes attributes = xml.attributes();

		if (xml.name() == "title") {
			xml.readNext();  //skip to text value
			QString text = xml.text().toString();

			if (!text.trimmed().isEmpty()) {
				std::cout << "title : " 
					<< text.toStdString() << std::endl;
			}
		}

		if (xml.name() == "content" ) {
			xml.readNext();  //get to text value

			if(xml.tokenType() != QXmlStreamReader::Characters) {
				printf(" not characters \n");
			}

			QString text = xml.text().toString();

			if (!text.trimmed().isEmpty()) {
				std::cout << "content : " 
					<< text.toStdString() << std::endl ;
			} else {
				std::cout << "content empty\n" ;
			}
		}

		if (xml.name() == "link") 
			parseLink(xml);


	}

}

int main()
{
	curl_global_init( CURL_GLOBAL_ALL );
	CURL * myHandle;

	CURLcode result; // We’ll store the result of CURL’s webpage retrieval, for simple error checking.
	myHandle = curl_easy_init ( ) ;

	// Notice the lack of major error checking, for brevity

	//curl_easy_setopt(myHandle, CURLOPT_URL, "http://localhost:8080/opds");
	//curl_easy_setopt(myHandle, CURLOPT_URL, "http://localhost:8080/opds/category/736572696573/49333a736572696573");
	curl_easy_setopt(myHandle, CURLOPT_URL, "http://localhost:8080/opds/navcatalog/4e736572696573?offset=30");

	curl_easy_setopt(myHandle, CURLOPT_WRITEFUNCTION, readData);

	curl_easy_setopt(myHandle, CURLOPT_WRITEDATA, &data);

	result = curl_easy_perform( myHandle );


	curl_easy_cleanup( myHandle );

	std::cout << data.toStdString() << std::endl;

	//try to get through xml

	if (!data.isEmpty()) {
		QXmlStreamReader xml(data);

		QXmlStreamReader::TokenType token;
		QXmlStreamAttributes attributes ;


		while ((token = xml.readNext()) != QXmlStreamReader::Invalid) {
			attributes = xml.attributes();
			//only entry elements hold interesting data
			if (xml.name() == "entry") { 
				readEntry(xml);
			}
			if (xml.name() == "link") 
				parseLink(xml);
		}
	}

	return 0;
}




