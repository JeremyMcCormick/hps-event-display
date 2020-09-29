#ifndef HPS_EVENTDISPLAY_H_
#define HPS_EVENTDISPLAY_H_ 1

// HPS
#include "Logger.h"

// ROOT
#include "TGFrame.h"
#include "TEveManager.h"
#include "TGNumberEntry.h"

// C++ standard library
#include <vector>
#include <string>

namespace hps {

    class EventManager;
    class DetectorGeometry;
    class FileCache;

    class EventDisplay : public TGMainFrame, public Logger {

        public:

            virtual ~EventDisplay();

            static EventDisplay* getInstance();

            void initialize();

            void setEveManager(TEveManager*);

            void setGeometryFile(std::string);

            void setCacheDir(std::string);

            void addLcioFiles(std::vector<std::string>);

            void addExcludeCollections(std::set<std::string>);

            void setMagFieldY(double);

            EventManager* getEventManager();

            TEveManager* getEveManager();

            DetectorGeometry* getDetectorGeometry();

            const std::vector<std::string>& getLcioFiles();

            bool excludeCollection(const std::string& collName);

            /**
             * Get current event number from GUI component.
             */
            int getCurrentEventNumber();

            double getMagFieldY();

            double getMCPCut();

            double getTrackPCut();

        private:

            void buildGUI();

            EventDisplay();

        private:

            static EventDisplay* instance_;

            std::string geometryFile_;
            std::string cacheDir_;

            FileCache* cache_;

            TEveManager* eveManager_;
            //TEveBrowser* browser_;
            EventManager* eventManager_;
            DetectorGeometry* det_;


            std::vector<std::string> lcioFileList_;
            std::set<std::string> excludeColls_;

            double bY_{0.};

            TGNumberEntry* eventNumberEntry_;
            TGNumberEntry* MCParticlePCutEntry_;
            TGNumberEntry* TrackPCutEntry_;

            ClassDef(EventDisplay, 1);
    };
}

#endif
