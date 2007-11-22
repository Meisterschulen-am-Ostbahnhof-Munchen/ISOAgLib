vt : main.o vtobject_c.o vtobjectalarmmask_c.o vtobjectbutton_c.o vtobjectcontainer_c.o vtobjectdatamask_c.o vtobjectfontattributes_c.o vtobjectinputboolean_c.o vtobjectinputlist_c.o vtobjectinputnumber_c.o vtobjectkey_c.o vtobjectline_c.o vtobjectlinearbargraph_c.o vtobjectlineattributes_c.o vtobjectnumbervariable_c.o vtobjectobjectpointer_c.o vtobjectoutputnumber_c.o vtobjectoutputstring_c.o vtobjectpicturegraphic_c.o vtobjectsoftkeymask_c.o vtobjectstringvariable_c.o vtobjectworkingset_c.o
	g++ -o vt main.o vtobject_c.o vtobjectalarmmask_c.o vtobjectbutton_c.o vtobjectcontainer_c.o vtobjectdatamask_c.o vtobjectfontattributes_c.o vtobjectinputboolean_c.o vtobjectinputlist_c.o vtobjectinputnumber_c.o vtobjectkey_c.o vtobjectline_c.o vtobjectlinearbargraph_c.o vtobjectlineattributes_c.o vtobjectnumbervariable_c.o vtobjectobjectpointer_c.o vtobjectoutputnumber_c.o vtobjectoutputstring_c.o vtobjectpicturegraphic_c.o vtobjectsoftkeymask_c.o vtobjectstringvariable_c.o vtobjectworkingset_c.o
	echo && echo && echo "Starting testprogram './vt'" && ./vt

main.o : main.cpp vttypes.h test.xml-variables.inc test.xml-attributes.inc test.xml-functions.inc
	g++ -c main.cpp

vtobject_c.o : vtobject_c.cpp vtobject_c.h
	g++ -c vtobject_c.cpp

vtobjectalarmmask_c.o : vtobjectalarmmask_c.cpp vtobjectalarmmask_c.h
	g++ -c vtobjectalarmmask_c.cpp

vtobjectbutton_c.o : vtobjectbutton_c.cpp vtobjectbutton_c.h
	g++ -c vtobjectbutton_c.cpp

vtobjectcontainer_c.o : vtobjectcontainer_c.cpp vtobjectcontainer_c.h
	g++ -c vtobjectcontainer_c.cpp

vtobjectdatamask_c.o : vtobjectdatamask_c.cpp vtobjectdatamask_c.h
	g++ -c vtobjectdatamask_c.cpp 

vtobjectfontattributes_c.o : vtobjectfontattributes_c.cpp vtobjectfontattributes_c.h
	g++ -c vtobjectfontattributes_c.cpp

vtobjectinputboolean_c.o : vtobjectinputboolean_c.cpp vtobjectinputboolean_c.h
	g++ -c vtobjectinputboolean_c.cpp

vtobjectinputlist_c.o : vtobjectinputlist_c.cpp vtobjectinputlist_c.h
	g++ -c vtobjectinputlist_c.cpp

vtobjectinputnumber_c.o : vtobjectinputnumber_c.cpp vtobjectinputnumber_c.h
	g++ -c vtobjectinputnumber_c.cpp

vtobjectkey_c.o : vtobjectkey_c.cpp vtobjectkey_c.h
	g++ -c vtobjectkey_c.cpp

vtobjectline_c.o : vtobjectline_c.cpp vtobjectline_c.h
	g++ -c vtobjectline_c.cpp

vtobjectlinearbargraph_c.o : vtobjectlinearbargraph_c.cpp vtobjectlinearbargraph_c.h
	g++ -c vtobjectlinearbargraph_c.cpp

vtobjectlineattributes_c.o : vtobjectlineattributes_c.cpp vtobjectlineattributes_c.h
	g++ -c vtobjectlineattributes_c.cpp

vtobjectnumbervariable_c.o : vtobjectnumbervariable_c.cpp  vtobjectnumbervariable_c.h
	g++ -c vtobjectnumbervariable_c.cpp 

vtobjectobjectpointer_c.o : vtobjectobjectpointer_c.cpp vtobjectobjectpointer_c.h
	g++ -c vtobjectobjectpointer_c.cpp

vtobjectoutputnumber_c.o : vtobjectoutputnumber_c.cpp vtobjectoutputnumber_c.h
	g++ -c vtobjectoutputnumber_c.cpp

vtobjectoutputstring_c.o : vtobjectoutputstring_c.cpp vtobjectoutputstring_c.h
	g++ -c vtobjectoutputstring_c.cpp

vtobjectpicturegraphic_c.o : vtobjectpicturegraphic_c.cpp vtobjectpicturegraphic_c.h
	g++ -c vtobjectpicturegraphic_c.cpp

vtobjectsoftkeymask_c.o : vtobjectsoftkeymask_c.cpp vtobjectsoftkeymask_c.h
	g++ -c vtobjectsoftkeymask_c.cpp

vtobjectstringvariable_c.o : vtobjectstringvariable_c.cpp vtobjectstringvariable_c.h
	g++ -c vtobjectstringvariable_c.cpp

vtobjectworkingset_c.o : vtobjectworkingset_c.cpp vtobjectworkingset_c.h
	g++ -c vtobjectworkingset_c.cpp

clean :
	rm vt *.o
