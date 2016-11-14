all: RxFusion.zip

.PHONY: RxFusion.zip
RxFusion.zip:
	-rm RxFusion.zip
	-rm -rf release
	mkdir -p release/src
	cp -r ../include release/src
	cp RxFusion.h release/src
	cp library.properties release
	cd release; zip -r ../RxFusion.zip * --exclude '*~'
