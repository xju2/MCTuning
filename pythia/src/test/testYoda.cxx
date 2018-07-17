#include "YODA/ReaderYODA.h"
#include "YODA/Histo2D.h"
#include <fstream>

using namespace std;
int main(int argc, char** argv){
	if(argc < 2) {
		cout << argv[0] << " yoda_file" << endl;
		return 1;
	}
	std::fstream fs;
	fs.open(argv[1], std::fstream::in | fstream::out);
	YODA::Reader& reader = YODA::ReaderYODA::create();

	vector<YODA::AnalysisObject*> ana_objects;
	reader.read(fs, ana_objects); 
	cout <<"total objects: " << ana_objects.size() << endl;
	
	if (ana_objects.size() > 0){
		auto& jrs = ana_objects[0];
		cout << "type: " << jrs->type() << endl;
		YODA::Histo2D* jrs_2d = dynamic_cast<YODA::Histo2D*>(ana_objects[0]);
		cout << "title: " <<  jrs_2d->title() << endl;
		cout << "path: " << jrs_2d->path() << endl;
		cout << "nbins: " << jrs_2d->numBins() << endl;
		cout << "nbinsX: " << jrs_2d->numBinsX() << endl;
		cout << "nbinsY: " << jrs_2d->numBinsY() << endl;
		for(int i = 0; i < jrs_2d->numBins(); i ++ ) {
			auto bin_info = jrs_2d->bin(i);
			cout << "\tbin: " << i << " X " << bin_info.xMin() << ", " \
				<< bin_info.xMax() << ";Y " << bin_info.yMin() << ", " \
				<< bin_info.yMax() << ", "
				<< bin_info.height() << " " << bin_info.volume() << endl;
		}
		int bin = jrs_2d->binIndexAt(0.3, 500);
		cout << "bin: " << bin << " " << jrs_2d->bin(bin).yMax() << endl;
	}

	fs.close();
	return 0;
}
