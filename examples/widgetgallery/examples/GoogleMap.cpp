#include <Wt/WCheckBox>
#include <Wt/WComboBox>
#include <Wt/WContainerWidget>
#include <Wt/WHBoxLayout>
#include <Wt/WGoogleMap>
#include <Wt/WPushButton>
#include <Wt/WStandardItemModel>
#include <Wt/WTemplate>

class GoogleMapExample : public Wt::WContainerWidget
{
public:
    GoogleMapExample(Wt::WContainerWidget *parent = 0)
	: Wt::WContainerWidget(parent)
    {
	Wt::WHBoxLayout *layout = new Wt::WHBoxLayout();
	setLayout(layout);

	setHeight(400);

	map_ = new Wt::WGoogleMap(Wt::WGoogleMap::Version3);
	layout->addWidget(map_, 1);

	map_->setMapTypeControl(Wt::WGoogleMap::DefaultControl);
	map_->enableScrollWheelZoom();

	Wt::WTemplate *controls =
	    new Wt::WTemplate(tr("graphics-GoogleMap-controls"));
	layout->addWidget(controls);

	Wt::WPushButton *zoomIn = new Wt::WPushButton("+");
	zoomIn->addStyleClass("zoom");
	controls->bindWidget("zoom-in", zoomIn);

	zoomIn->clicked().connect(std::bind([=] () {
	    map_->zoomIn();
	}));

	Wt::WPushButton *zoomOut = new Wt::WPushButton("-");
	zoomOut->addStyleClass("zoom");
	controls->bindWidget("zoom-out", zoomOut);

	zoomOut->clicked().connect(std::bind([=] () {
            map_->zoomOut();
	}));

	std::string cityNames[] = { "Brussels", "Lisbon", "Paris" };
	static Wt::WGoogleMap::Coordinate cityCoords[] = {
	    Wt::WGoogleMap::Coordinate(50.85034,4.35171),
	    Wt::WGoogleMap::Coordinate(38.703731,-9.135475),
	    Wt::WGoogleMap::Coordinate(48.877474, 2.312579)
	};
	    
	for (unsigned i = 0; i < 3; ++i) {
	    Wt::WPushButton *city = new Wt::WPushButton(cityNames[i]);
	    controls->bindWidget(cityNames[i], city);

	    city->clicked().connect(std::bind([=] () {
		map_->panTo(cityCoords[i]);
	    }));
	}

	Wt::WPushButton *reset = new Wt::WPushButton("Reset");
	controls->bindWidget("emweb", reset);

	reset->clicked().connect(std::bind([=] () {
            this->panToEmWeb();
        }));

	Wt::WPushButton *savePosition =
	    new Wt::WPushButton("Save current position");
	controls->bindWidget("save-position", savePosition);

	savePosition->clicked().connect(std::bind([=] () {
            this->savePosition();
        }));

	returnToPosition_ = new Wt::WPushButton("Return to saved position");
	controls->bindWidget("return-to-saved-position", returnToPosition_);
	returnToPosition_->setEnabled(false);

	returnToPosition_->clicked().connect(std::bind([=] () {
            map_->returnToSavedPosition();
        }));

	mapTypeModel_ = new Wt::WStandardItemModel();
	addMapTypeControl("No control", Wt::WGoogleMap::NoControl);
	addMapTypeControl("Default", Wt::WGoogleMap::DefaultControl);
	addMapTypeControl("Menu", Wt::WGoogleMap::MenuControl);
	if (map_->apiVersion() == Wt::WGoogleMap::Version2)
	    addMapTypeControl("Hierarchical",
			      Wt::WGoogleMap::HierarchicalControl);
	if (map_->apiVersion() == Wt::WGoogleMap::Version3)
	    addMapTypeControl("Horizontal bar",
			      Wt::WGoogleMap::HorizontalBarControl);

	Wt::WComboBox* menuControls = new Wt::WComboBox();
	menuControls->setModel(mapTypeModel_);
	menuControls->setCurrentIndex(1);
	controls->bindWidget("control-menu-combo", menuControls);

	menuControls->activated().connect(std::bind([=] (int mapType) {
            this->setMapTypeControl(mapType);
        }, std::placeholders::_1));

	Wt::WCheckBox *draggingCB = new Wt::WCheckBox("Enable dragging");
	controls->bindWidget("dragging-cb", draggingCB);
	draggingCB->setChecked(true);
	map_->enableDragging();

	draggingCB->checked().connect(std::bind([=] () {
            map_->enableDragging();
        }));

	draggingCB->unChecked().connect(std::bind([=] () {
            map_->disableDragging();
        }));

	Wt::WCheckBox *enableDoubleClickZoomCB =
            new Wt::WCheckBox("Enable double click zoom");
	controls->bindWidget("double-click-zoom-cb", enableDoubleClickZoomCB);
	enableDoubleClickZoomCB->setChecked(false);
	map_->disableDoubleClickZoom();

	enableDoubleClickZoomCB->checked().connect(std::bind([=] () {
            map_->enableDoubleClickZoom();
	}));

	enableDoubleClickZoomCB->unChecked().connect(std::bind([=] () {
            map_->disableDoubleClickZoom();
        }));

	Wt::WCheckBox *enableScrollWheelZoomCB =
            new Wt::WCheckBox("Enable scroll wheel zoom");
	controls->bindWidget("scroll-wheel-zoom-cb", enableScrollWheelZoomCB);
	enableScrollWheelZoomCB->setChecked(true);
	map_->enableScrollWheelZoom();

	enableScrollWheelZoomCB->checked().connect(std::bind([=] () {
            map_->enableScrollWheelZoom();
        }));

	enableScrollWheelZoomCB->unChecked().connect(std::bind([=] () {
            map_->disableScrollWheelZoom();
        }));

	std::vector<Wt::WGoogleMap::Coordinate> road = roadDescription();

	map_->addPolyline(road, Wt::WColor(0, 191, 255));

	//Koen's favourite bar!
	map_->addMarker(Wt::WGoogleMap::Coordinate(50.885069,4.71958));

	map_->setCenter(road[road.size()-1]);

	map_->openInfoWindow(road[0],
           "<img src=\"http://www.emweb.be/css/emweb_small.jpg\" />"
           "<p><strong>Emweb office</strong></p>");

	map_->clicked().connect(std::bind([=] (Wt::WGoogleMap::Coordinate c) {
            this->googleMapClicked(c);
        }, std::placeholders::_1));

	map_->doubleClicked().connect
	    (std::bind([=] (Wt::WGoogleMap::Coordinate c) {
            this->googleMapDoubleClicked(c);
        }, std::placeholders::_1));
    }

private:
    void panToEmWeb() {
        map_->panTo(Wt::WGoogleMap::Coordinate(50.9082, 4.66056));
    }

    void savePosition() {
        returnToPosition_->setEnabled(true);
        map_->savePosition();
    }

    void addMapTypeControl(const Wt::WString &description,
			   Wt::WGoogleMap::MapTypeControl value) {
	int r = mapTypeModel_->rowCount();
	mapTypeModel_->insertRow(r);
	mapTypeModel_->setData(r, 0, description);
	mapTypeModel_->setData(r, 0, value, Wt::UserRole);
    }

    void setMapTypeControl(int row) {
	boost::any mtc = mapTypeModel_->data(row, 0, Wt::UserRole);
	map_->setMapTypeControl(boost::any_cast<Wt::WGoogleMap::MapTypeControl>
				(mtc));
    }
    
    std::vector<Wt::WGoogleMap::Coordinate> roadDescription() {
	std::vector<Wt::WGoogleMap::Coordinate> result;
	result.push_back(Wt::WGoogleMap::Coordinate(50.9082, 4.66056));
	result.push_back(Wt::WGoogleMap::Coordinate(50.90901, 4.66426));
	result.push_back(Wt::WGoogleMap::Coordinate(50.90944, 4.66514));
	result.push_back(Wt::WGoogleMap::Coordinate(50.90968, 4.66574));
	result.push_back(Wt::WGoogleMap::Coordinate(50.91021, 4.66541));
	result.push_back(Wt::WGoogleMap::Coordinate(50.9111, 4.66508));
	result.push_back(Wt::WGoogleMap::Coordinate(50.9119, 4.66469));
	result.push_back(Wt::WGoogleMap::Coordinate(50.91224, 4.66463));
	result.push_back(Wt::WGoogleMap::Coordinate(50.91227, 4.66598));
	result.push_back(Wt::WGoogleMap::Coordinate(50.9122, 4.66786));
	result.push_back(Wt::WGoogleMap::Coordinate(50.91199, 4.66962));
	result.push_back(Wt::WGoogleMap::Coordinate(50.91169, 4.67117));
	result.push_back(Wt::WGoogleMap::Coordinate(50.91107, 4.67365));
	result.push_back(Wt::WGoogleMap::Coordinate(50.91061, 4.67515));
	result.push_back(Wt::WGoogleMap::Coordinate(50.91023, 4.67596));
	result.push_back(Wt::WGoogleMap::Coordinate(50.9098, 4.67666));
	result.push_back(Wt::WGoogleMap::Coordinate(50.90953, 4.67691));
	result.push_back(Wt::WGoogleMap::Coordinate(50.90912, 4.67746));
	result.push_back(Wt::WGoogleMap::Coordinate(50.90882, 4.67772));
	result.push_back(Wt::WGoogleMap::Coordinate(50.90838, 4.67801));
	result.push_back(Wt::WGoogleMap::Coordinate(50.9083, 4.67798));
	result.push_back(Wt::WGoogleMap::Coordinate(50.90803, 4.67814));
	result.push_back(Wt::WGoogleMap::Coordinate(50.90742, 4.67836));
	result.push_back(Wt::WGoogleMap::Coordinate(50.90681, 4.67845));
	result.push_back(Wt::WGoogleMap::Coordinate(50.90209, 4.67871));
	result.push_back(Wt::WGoogleMap::Coordinate(50.90134, 4.67893));
	result.push_back(Wt::WGoogleMap::Coordinate(50.90066, 4.6793));
	result.push_back(Wt::WGoogleMap::Coordinate(50.90015, 4.67972));
	result.push_back(Wt::WGoogleMap::Coordinate(50.89945, 4.68059));
	result.push_back(Wt::WGoogleMap::Coordinate(50.89613, 4.68582));
	result.push_back(Wt::WGoogleMap::Coordinate(50.8952, 4.68719));
	result.push_back(Wt::WGoogleMap::Coordinate(50.89464, 4.68764));
	result.push_back(Wt::WGoogleMap::Coordinate(50.89183, 4.69032));
	result.push_back(Wt::WGoogleMap::Coordinate(50.89131, 4.69076));
	result.push_back(Wt::WGoogleMap::Coordinate(50.88916, 4.69189));
	result.push_back(Wt::WGoogleMap::Coordinate(50.88897, 4.69195));
	result.push_back(Wt::WGoogleMap::Coordinate(50.88859, 4.69195));
	result.push_back(Wt::WGoogleMap::Coordinate(50.88813, 4.69193));
	result.push_back(Wt::WGoogleMap::Coordinate(50.88697, 4.69135));
	result.push_back(Wt::WGoogleMap::Coordinate(50.88669, 4.6913));
	result.push_back(Wt::WGoogleMap::Coordinate(50.88531, 4.69155));
	result.push_back(Wt::WGoogleMap::Coordinate(50.88425, 4.69196));
	result.push_back(Wt::WGoogleMap::Coordinate(50.88398, 4.69219));
	result.push_back(Wt::WGoogleMap::Coordinate(50.88391, 4.69226));
	result.push_back(Wt::WGoogleMap::Coordinate(50.88356, 4.69292));
	result.push_back(Wt::WGoogleMap::Coordinate(50.88323, 4.69361));
	result.push_back(Wt::WGoogleMap::Coordinate(50.88067, 4.6934));
	result.push_back(Wt::WGoogleMap::Coordinate(50.88055, 4.69491));
	result.push_back(Wt::WGoogleMap::Coordinate(50.88036, 4.69616));
	result.push_back(Wt::WGoogleMap::Coordinate(50.88009, 4.69755));
	result.push_back(Wt::WGoogleMap::Coordinate(50.87973, 4.69877));
	result.push_back(Wt::WGoogleMap::Coordinate(50.87951, 4.69856));
	result.push_back(Wt::WGoogleMap::Coordinate(50.87933, 4.69831));
	result.push_back(Wt::WGoogleMap::Coordinate(50.87905, 4.69811));
	result.push_back(Wt::WGoogleMap::Coordinate(50.879, 4.69793));
	result.push_back(Wt::WGoogleMap::Coordinate(50.87856, 4.69745));
	result.push_back(Wt::WGoogleMap::Coordinate(50.87849, 4.69746));
	result.push_back(Wt::WGoogleMap::Coordinate(50.87843, 4.69758));
	result.push_back(Wt::WGoogleMap::Coordinate(50.87822, 4.69758));
	result.push_back(Wt::WGoogleMap::Coordinate(50.87814, 4.69766));
	result.push_back(Wt::WGoogleMap::Coordinate(50.87813, 4.69788));
	result.push_back(Wt::WGoogleMap::Coordinate(50.87789, 4.69862));
	
	return result;
    }

    void googleMapDoubleClicked(Wt::WGoogleMap::Coordinate c) {
	std::cerr << "Double clicked at coordinate ("
		  << c.latitude() << "," << c.longitude() << ")";
    }

    void googleMapClicked(Wt::WGoogleMap::Coordinate c) {
	std::cerr << "Clicked at coordinate ("
		  << c.latitude() << "," << c.longitude() << ")";
    }

private:
    Wt::WGoogleMap *map_;
    Wt::WAbstractItemModel *mapTypeModel_;

    Wt::WPushButton *returnToPosition_;
};

SAMPLE_BEGIN(GoogleMap)
GoogleMapExample *map = new GoogleMapExample();

SAMPLE_END(return map)
