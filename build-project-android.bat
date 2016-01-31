rm -rf android-build
mkdir android-build
cd android-build
qmake CONFIG+="release" ANDROID_PACKAGE_SOURCE_DIR+="../android" ..
mingw32-make
mingw32-make INSTALL_ROOT=build install
androiddeployqt.exe --input android-libeuler.so-deployment-settings.json --output build --ant D:/adt-bundle-windows-x86_64-20130717/eclipse/plugins/org.apache.ant_1.8.3.v201301120609/bin/ant.bat --jdk "C:/Program Files/Java/jdk1.7.0_51"
cd ..
