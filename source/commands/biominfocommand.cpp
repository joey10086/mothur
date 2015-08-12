//
//  biominfocommand.cpp
//  Mothur
//
//  Created by Sarah Westcott on 8/5/15.
//  Copyright (c) 2015 Schloss Lab. All rights reserved.
//

#include "biominfocommand.h"
#include "sharedutilities.h"


//**********************************************************************************************************************
vector<string> BiomInfoCommand::setParameters(){
    try {
        CommandParameter pbiom("biom", "InputTypes", "", "", "", "", "","",false,true, true); parameters.push_back(pbiom);
        CommandParameter plabel("label", "String", "", "", "", "", "","",false,false); parameters.push_back(plabel);
         CommandParameter pseed("seed", "Number", "", "0", "", "", "","",false,false); parameters.push_back(pseed);
        CommandParameter pinputdir("inputdir", "String", "", "", "", "", "","",false,false); parameters.push_back(pinputdir);
        CommandParameter poutputdir("outputdir", "String", "", "", "", "", "","",false,false); parameters.push_back(poutputdir);
        
        vector<string> myArray;
        for (int i = 0; i < parameters.size(); i++) {	myArray.push_back(parameters[i].name);		}
        return myArray;
    }
    catch(exception& e) {
        m->errorOut(e, "BiomInfoCommand", "setParameters");
        exit(1);
    }
}
//**********************************************************************************************************************
string BiomInfoCommand::getHelpString(){
    try {
        string helpString = "";
        helpString += "The biom.info command reads a biom file creates a shared file. If your biom file contains metadata mothur will also create taxonomy or constaxonomy files.\n";
        helpString += "The biom.info command parameters are biom and label. The biom parameter is required.\n";
        helpString += "The label parameter allows you to enter a distance label to be used in the shared file created from your biom file.\n";
        helpString += "The biom.info command should be in the following format: biom.info(biom=test.biom, label=0.03).\n";
        helpString += "Note: No spaces between parameter labels (i.e. label), '=' and parameters (i.e. 0.03).\n";
        return helpString;
    }
    catch(exception& e) {
        m->errorOut(e, "BiomInfoCommand", "getHelpString");
        exit(1);
    }
}
//**********************************************************************************************************************
string BiomInfoCommand::getOutputPattern(string type) {
    try {
        string pattern = "";
        
        if (type == "shared") {  pattern = "[filename],[tag],shared"; }
        else if (type == "constaxonomy") {  pattern = "[filename],[tag],cons.taxonomy"; }
        else if (type == "taxonomy") {  pattern = "[filename],[tag],taxonomy"; }
        else if (type == "taxsummary") {  pattern = "[filename],[tag],[tag2],tax.summary"; } //tag2 = "" for taxonomy tag2 = cons for constaxonomy
        else { m->mothurOut("[ERROR]: No definition for type " + type + " output pattern.\n"); m->control_pressed = true;  }
        
        return pattern;
    }
    catch(exception& e) {
        m->errorOut(e, "BiomInfoCommand", "getOutputPattern");
        exit(1);
    }
}
//**********************************************************************************************************************
BiomInfoCommand::BiomInfoCommand(){
    try {
        abort = true; calledHelp = true;
        setParameters();
        vector<string> tempOutNames;
        outputTypes["taxonomy"] = tempOutNames;
        outputTypes["shared"] = tempOutNames;
        outputTypes["constaxonomy"] = tempOutNames;
        outputTypes["taxsummary"] = tempOutNames;
    }
    catch(exception& e) {
        m->errorOut(e, "BiomInfoCommand", "BiomInfoCommand");
        exit(1);
    }
}
//**********************************************************************************************************************
BiomInfoCommand::BiomInfoCommand(string option)  {
    try {
        abort = false; calledHelp = false; pickedGroups=false;
        
        //allow user to run help
        if(option == "help") { help(); abort = true; calledHelp = true; }
        else if(option == "citation") { citation(); abort = true; calledHelp = true;}
        
        else {
            
            vector<string> myArray = setParameters();
            
            OptionParser parser(option);
            map<string, string> parameters = parser.getParameters();
            
            ValidParameters validParameter;
            map<string, string>::iterator it;
            
            //check to make sure all parameters are valid for command
            for (it = parameters.begin(); it != parameters.end(); it++) {
                if (validParameter.isValidParameter(it->first, myArray, it->second) != true) {  abort = true;  }
            }
            
            //if the user changes the input directory command factory will send this info to us in the output parameter
            string inputDir = validParameter.validFile(parameters, "inputdir", false);
            if (inputDir == "not found"){	inputDir = "";		}
            else {
                string path;
                it = parameters.find("biom");
                //user has given a template file
                if(it != parameters.end()){
                    path = m->hasPath(it->second);
                    //if the user has not given a path then, add inputdir. else leave path alone.
                    if (path == "") {	parameters["biom"] = inputDir + it->second;		}
                }
            }
            
            vector<string> tempOutNames;
            outputTypes["taxonomy"] = tempOutNames;
            outputTypes["shared"] = tempOutNames;
            outputTypes["constaxonomy"] = tempOutNames;
            outputTypes["taxsummary"] = tempOutNames;
            
            //if the user changes the output directory command factory will send this info to us in the output parameter
            outputDir = validParameter.validFile(parameters, "outputdir", false);		if (outputDir == "not found"){	outputDir = "";	}
            
            //check for required parameters
            biomfile = validParameter.validFile(parameters, "biom", true);
            if (biomfile == "not open") { biomfile = ""; abort = true; }
            else if (biomfile == "not found") { biomfile = ""; m->mothurOut("[ERROR]: You must provide a biom file, please correct.\n");  abort = true;}
            else { m->setBiomFile(biomfile); }
            
            label = validParameter.validFile(parameters, "label", false);
            if (label == "not found") { label = "userLabel"; }
        }
        
    }
    catch(exception& e) {
        m->errorOut(e, "BiomInfoCommand", "BiomInfoCommand");
        exit(1);
    }
}
//**********************************************************************************************************************

int BiomInfoCommand::execute(){
    try {
        
        if (abort == true) { if (calledHelp) { return 0; }  return 2;	}
        
        createSharedFromBiom();
        
        if (m->control_pressed) {
            for (int i = 0; i < outputNames.size(); i++) {
                m->mothurRemove(outputNames[i]);
            }
        }
        
        string current = "";
        itTypes = outputTypes.find("shared");
        if (itTypes != outputTypes.end()) {
            if ((itTypes->second).size() != 0) { current = (itTypes->second)[0]; m->setSharedFile(current); }
        }
        
        m->mothurOutEndLine();
        m->mothurOut("Output File Names: "); m->mothurOutEndLine();
        for (int i = 0; i < outputNames.size(); i++) {	m->mothurOut(outputNames[i]); m->mothurOutEndLine();	}
        m->mothurOutEndLine();
        
        return 0;
    }
    catch(exception& e) {
        m->errorOut(e, "BiomInfoCommand", "execute");
        exit(1);
    }
}
//**********************************************************************************************************************
int BiomInfoCommand::createSharedFromBiom() {
    try {
        //getting output filename
        string filename = biomfile;
        if (outputDir == "") { outputDir += m->hasPath(filename); }
        
        map<string, string> variables;
        variables["[filename]"] = outputDir + m->getRootName(m->getSimpleName(filename));
        variables["[tag]"] = label;
        filename = getOutputFileName("shared",variables);
        outputNames.push_back(filename); outputTypes["shared"].push_back(filename);
        
        ofstream out;
        m->openOutputFile(filename, out);
        
        /*{
         "id":"/Users/SarahsWork/Desktop/release/temp.job2.shared-unique",
         "format": "Biological Observation Matrix 0.9.1",
         "format_url": "http://biom-format.org",
         "type": "OTU table",
         "generated_by": "mothur1.24.0",
         "date": "Tue Apr 17 13:12:07 2012", */
        
        ifstream in;
        m->openInputFile(biomfile, in);
        
        string matrixFormat = "";
        int numRows = 0;
        int numCols = 0;
        int shapeNumRows = 0;
        int shapeNumCols = 0;
        vector<string> otuNames;
        vector<string> groupNames;
        map<string, string> fileLines;
        vector<string> names;
        int countOpenBrace = 0;
        int countClosedBrace = 0;
        int openParen = -1; //account for opening brace
        int closeParen = 0;
        bool ignoreCommas = false;
        bool atComma = false;
        string line = "";
        string matrixElementType = "";
        
        while (!in.eof()) { //split file by tags, so each "line" will have something like "id":"/Users/SarahsWork/Desktop/release/final.tx.1.subsample.1.pick.shared-1"
            if (m->control_pressed) { break; }
            
            char c = in.get(); m->gobble(in);
            
            if (c == '[')               { countOpenBrace++;     }
            else if (c == ']')          { countClosedBrace++;   }
            else if (c == '{')          { openParen++;          }
            else if (c == '}')          { closeParen++;         }
            else if ((!ignoreCommas) && (c == ','))          { atComma = true;       }
            
            if ((countOpenBrace != countClosedBrace) && (countOpenBrace != countClosedBrace)) { ignoreCommas = true;  }
            else if ((countOpenBrace == countClosedBrace) && (countOpenBrace == countClosedBrace)) { ignoreCommas = false;  }
            if (atComma && !ignoreCommas) {
                if (fileLines.size() == 0) { //clip first {
                    line = line.substr(1);
                }
                string tag = getTag(line);
                fileLines[tag] = line;
                
                line = "";
                atComma = false;
                ignoreCommas = false;
                
            }else {  line += c;  }
            
        }
        if (line != "") {
            line = line.substr(0, line.length()-1);
            string tag = getTag(line);
            fileLines[tag] = line;
            
        }
        in.close();
        
        string biomType;
        map<string, string>::iterator it;
        it = fileLines.find("type");
        if (it == fileLines.end()) { m->mothurOut("[ERROR]: you file does not have a type provided.\n"); }
        else {
            string thisLine = it->second;
            biomType = getTag(thisLine);
            //            if ((biomType != "OTU table") && (biomType != "OTUtable") && (biomType != "Taxon table") && (biomType != "Taxontable")) { m->mothurOut("[ERROR]: " + biomType + " is not a valid biom type for mothur. Only types allowed are OTU table and Taxon table.\n"); m->control_pressed = true;  }
        }
        
        if (m->control_pressed) { out.close(); m->mothurRemove(filename); return 0; }
        
        it = fileLines.find("matrix_type");
        if (it == fileLines.end()) { m->mothurOut("[ERROR]: you file does not have a matrix_type provided.\n"); }
        else {
            string thisLine = it->second;
            matrixFormat = getTag(thisLine);
            if ((matrixFormat != "sparse") && (matrixFormat != "dense")) { m->mothurOut("[ERROR]: " + matrixFormat + " is not a valid biom matrix_type for mothur. Types allowed are sparse and dense.\n"); m->control_pressed = true; }
        }
        
        if (m->control_pressed) { out.close(); m->mothurRemove(filename); return 0; }
        
        it = fileLines.find("matrix_element_type");
        if (it == fileLines.end()) { m->mothurOut("[ERROR]: you file does not have a matrix_element_type provided.\n"); }
        else {
            string thisLine = it->second;
            matrixElementType = getTag(thisLine);
            if ((matrixElementType != "int") && (matrixElementType != "float")) { m->mothurOut("[ERROR]: " + matrixElementType + " is not a valid biom matrix_element_type for mothur. Types allowed are int and float.\n"); m->control_pressed = true; }
            if (matrixElementType == "float") { m->mothurOut("[WARNING]: the shared file only uses integers, any float values will be rounded down to the nearest integer.\n"); }
        }
        
        if (m->control_pressed) { out.close(); m->mothurRemove(filename); return 0; }
        
        vector<string> conTaxonomy;
        it = fileLines.find("rows");
        if (it == fileLines.end()) { m->mothurOut("[ERROR]: you file does not have a rows provided.\n"); }
        else {
            string thisLine = it->second;
            if ((biomType == "Taxon table") || (biomType == "Taxontable")) {
                string mapFilename = getOutputFileName("map",variables);
                outputNames.push_back(mapFilename); outputTypes["map"].push_back(mapFilename);
                ofstream outMap;
                m->openOutputFile(mapFilename, outMap);
                
                bool hasTaxonomy = false;
                vector< vector<string> > results = readRows(thisLine, numRows, hasTaxonomy);
                vector<string> taxonomies = results[0];
                
                string snumBins = toString(numRows);
                for (int i = 0; i < numRows; i++) {
                    
                    //if there is a bin label use it otherwise make one
                    string binLabel = "OTU";
                    string sbinNumber = toString(i+1);
                    if (sbinNumber.length() < snumBins.length()) {
                        int diff = snumBins.length() - sbinNumber.length();
                        for (int h = 0; h < diff; h++) { binLabel += "0"; }
                    }
                    binLabel += sbinNumber;
                    
                    otuNames.push_back(binLabel);
                    outMap << otuNames[i] << '\t' << taxonomies[i] << endl;
                }
                outMap.close();
            }else{
                bool hasTaxonomy = false;
                vector< vector<string> > results = readRows(thisLine, numRows, hasTaxonomy);
                otuNames = results[0];
                if (hasTaxonomy) { conTaxonomy = results[1]; }
            }
        }
        
        if (m->control_pressed) { out.close(); m->mothurRemove(filename); return 0; }
        
        it = fileLines.find("columns");
        if (it == fileLines.end()) { m->mothurOut("[ERROR]: you file does not have a columns provided.\n"); }
        else {
            string thisLine = it->second;
            
            //read sample names
            bool hasTaxonomy = false;
            vector< vector<string> > results = readRows(thisLine, numCols, hasTaxonomy);
            groupNames = results[0];
            if (hasTaxonomy) {
                /////////////////////write taxonomy file////////////////////////
                //////////////write tax.summary //////////////////
            }
            
            //if users selected groups, then remove the groups not wanted.
            SharedUtil util;
            vector<string> Groups = m->getGroups();
            vector<string> allGroups = groupNames;
            util.setGroups(Groups, allGroups);
            m->setGroups(Groups);
            
            //set fileroot
            fileroot = outputDir + m->getRootName(m->getSimpleName(biomfile));
        }
        
        if (m->control_pressed) {  out.close(); m->mothurRemove(filename); return 0; }
        
        it = fileLines.find("shape");
        if (it == fileLines.end()) { m->mothurOut("[ERROR]: you file does not have a shape provided.\n"); }
        else {
            string thisLine = it->second;
            getDims(thisLine, shapeNumRows, shapeNumCols);
            
            //check shape
            if (shapeNumCols != numCols) { m->mothurOut("[ERROR]: shape indicates " + toString(shapeNumCols) + " columns, but I only read " + toString(numCols) + " columns.\n"); m->control_pressed = true; }
            
            if (shapeNumRows != numRows) { m->mothurOut("[ERROR]: shape indicates " + toString(shapeNumRows) + " rows, but I only read " + toString(numRows) + " rows.\n"); m->control_pressed = true; }
        }
        
        if (m->control_pressed) {  out.close(); m->mothurRemove(filename); return 0; }
        
        it = fileLines.find("data");
        if (it == fileLines.end()) { m->mothurOut("[ERROR]: you file does not have a data provided.\n"); }
        else {
            string thisLine = it->second;
            m->currentSharedBinLabels = otuNames;
            
            //read data
            vector<SharedRAbundVector*> lookup = readData(matrixFormat, thisLine, matrixElementType, groupNames, otuNames.size());
            
            m->mothurOutEndLine(); m->mothurOut(lookup[0]->getLabel()); m->mothurOutEndLine();
            lookup[0]->printHeaders(out);
            printSharedData(lookup, out);
            
            if (conTaxonomy.size() != 0) {
                ////////////// write constaxonomy //////////////////
                //////////////write tax.summary //////////////////
            }
            
            for (int i = 0; i < lookup.size(); i++) {  delete lookup[i];  }
        }
        
        if (m->control_pressed) {  m->mothurRemove(filename); return 0; }
        
        return 0;
    }
    catch(exception& e) {
        m->errorOut(e, "BiomInfoCommand", "createSharedFromBiom");
        exit(1);
    }
}
//**********************************************************************************************************************
vector<SharedRAbundVector*> BiomInfoCommand::readData(string matrixFormat, string line, string matrixElementType, vector<string>& groupNames, int numOTUs) {
    try {
        
        vector<SharedRAbundVector*> lookup;
        
        //creates new sharedRAbunds
        for (int i = 0; i < groupNames.size(); i++) {
            SharedRAbundVector* temp = new SharedRAbundVector(numOTUs); //sets all abunds to 0
            temp->setLabel("userLabel");
            temp->setGroup(groupNames[i]);
            lookup.push_back(temp);
        }
        
        bool dataStart = false;
        bool inBrackets = false;
        string num = "";
        vector<int> nums;
        int otuCount = 0;
        for (int i = 0; i < line.length(); i++) {
            
            if (m->control_pressed) { return lookup; }
            
            //look for opening [ to indicate data is starting
            if ((line[i] == '[') && (!dataStart)) { dataStart = true; i++;  if (!(i < line.length())) { break; } }
            else if ((line[i] == ']') && dataStart && (!inBrackets)) { break; } //we are done reading data
            
            if (dataStart) {
                if ((line[i] == '[') && (!inBrackets)) { inBrackets = true; i++;  if (!(i < line.length())) { break; } }
                else if ((line[i] == ']') && (inBrackets)) {
                    inBrackets = false;
                    int temp;
                    float temp2;
                    if (matrixElementType == "float") { m->mothurConvert(num, temp2); temp = (int)temp2; }
                    else { m->mothurConvert(num, temp); }
                    nums.push_back(temp);
                    num = "";
                    
                    //save info to vectors
                    if (matrixFormat == "dense") {
                        
                        //sanity check
                        if (nums.size() != lookup.size()) { m->mothurOut("[ERROR]: trouble parsing OTU data.  OTU " + toString(otuCount) + " causing errors.\n"); m->control_pressed = true; }
                        
                        //set abundances for this otu
                        //nums contains [abundSample0, abundSample1, abundSample2, ...] for current OTU
                        for (int j = 0; j < lookup.size(); j++) { lookup[j]->set(otuCount, nums[j], groupNames[j]); }
                        
                        otuCount++;
                    }else {
                        //sanity check
                        if (nums.size() != 3) { m->mothurOut("[ERROR]: trouble parsing OTU data.\n"); m->control_pressed = true; }
                        
                        //nums contains [otuNum, sampleNum, abundance]
                        lookup[nums[1]]->set(nums[0], nums[2], groupNames[nums[1]]);
                    }
                    nums.clear();
                }
                
                if (inBrackets) {
                    if (line[i] == ',') {
                        int temp;
                        m->mothurConvert(num, temp);
                        nums.push_back(temp);
                        num = "";
                    }else { if (!isspace(line[i])) { num += line[i]; }  }
                }
            }
        }
        
        
        SharedUtil util;
        bool remove = false;
        if (pickedGroups) {
            for (int i = 0; i < lookup.size(); i++) {
                //if this sharedrabund is not from a group the user wants then delete it.
                if (util.isValidGroup(lookup[i]->getGroup(), m->getGroups()) == false) {
                    remove = true;
                    delete lookup[i]; lookup[i] = NULL;
                    lookup.erase(lookup.begin()+i);
                    i--;
                }
            }
        }
        
        if (remove) { eliminateZeroOTUS(lookup); }
        
        
        return lookup;
    }
    catch(exception& e) {
        m->errorOut(e, "BiomInfoCommand", "readData");
        exit(1);
    }
}
//**********************************************************************************************************************
int BiomInfoCommand::eliminateZeroOTUS(vector<SharedRAbundVector*>& thislookup) {
    try {
        
        vector<SharedRAbundVector*> newLookup;
        for (int i = 0; i < thislookup.size(); i++) {
            SharedRAbundVector* temp = new SharedRAbundVector();
            temp->setLabel(thislookup[i]->getLabel());
            temp->setGroup(thislookup[i]->getGroup());
            newLookup.push_back(temp);
        }
        
        //for each bin
        vector<string> newBinLabels;
        string snumBins = toString(thislookup[0]->getNumBins());
        for (int i = 0; i < thislookup[0]->getNumBins(); i++) {
            if (m->control_pressed) { for (int j = 0; j < newLookup.size(); j++) {  delete newLookup[j];  } return 0; }
            
            //look at each sharedRabund and make sure they are not all zero
            bool allZero = true;
            for (int j = 0; j < thislookup.size(); j++) {
                if (thislookup[j]->getAbundance(i) != 0) { allZero = false;  break;  }
            }
            
            //if they are not all zero add this bin
            if (!allZero) {
                for (int j = 0; j < thislookup.size(); j++) {
                    newLookup[j]->push_back(thislookup[j]->getAbundance(i), thislookup[j]->getGroup());
                }
                
                //if there is a bin label use it otherwise make one
                string binLabel = "Otu";
                string sbinNumber = toString(i+1);
                if (sbinNumber.length() < snumBins.length()) {
                    int diff = snumBins.length() - sbinNumber.length();
                    for (int h = 0; h < diff; h++) { binLabel += "0"; }
                }
                binLabel += sbinNumber;
                if (i < m->currentSharedBinLabels.size()) {  binLabel = m->currentSharedBinLabels[i]; }
                
                newBinLabels.push_back(binLabel);
            }
        }
        
        for (int j = 0; j < thislookup.size(); j++) {  delete thislookup[j];  }
        
        thislookup = newLookup;
        m->currentSharedBinLabels = newBinLabels;
        
        return 0;
        
    }
    catch(exception& e) {
        m->errorOut(e, "BiomInfoCommand", "eliminateZeroOTUS");
        exit(1);
    }
}
//**********************************************************************************************************************
int BiomInfoCommand::getDims(string line, int& shapeNumRows, int& shapeNumCols) {
    try {
        //get shape
        bool inBar = false;
        string num = "";
        
        for (int i = 0; i < line.length(); i++) {
            
            //you want to ignore any ; until you reach the next '
            if ((line[i] == '[') && (!inBar)) {  inBar = true; i++;  if (!(i < line.length())) { break; } }
            else if ((line[i] == ']') && (inBar)) {
                inBar= false;
                m->mothurConvert(num, shapeNumCols);
                break;
            }
            
            if (inBar) {
                if (line[i] == ',') {
                    m->mothurConvert(num, shapeNumRows);
                    num = "";
                }else { if (!isspace(line[i])) { num += line[i]; }  }
            }
        }
        
        return 0;
    }
    catch(exception& e) {
        m->errorOut(e, "BiomInfoCommand", "getDims");
        exit(1);
    }
}
//**********************************************************************************************************************
vector< vector<string> > BiomInfoCommand::readRows(string line, int& numRows, bool& allBlank) {
    try {
        /*"rows":[
         {"id":"Otu01", "metadata":{"taxonomy":["Bacteria", "Bacteroidetes", "Bacteroidia", "Bacteroidales", "Porphyromonadaceae", "unclassified"], "bootstrap":[100, 100, 100, 100, 100, 100]}},
         {"id":"Otu02", "metadata":{"taxonomy":["Bacteria", "Bacteroidetes", "Bacteroidia", "Bacteroidales", "Rikenellaceae", "Alistipes"], "bootstrap":[100, 100, 100, 100, 100, 100]}},
         ...
         
         "rows":[{"id": "k__Archaea;p__Euryarchaeota;c__Methanobacteria;o__Methanobacteriales;f__Methanobacteriaceae", "metadata": null},
         {"id": "k__Bacteria;p__Actinobacteria;c__Actinobacteria;o__Actinomycetales;f__Actinomycetaceae", "metadata": null}
         ....
         
         make look like above
         
         
         ],*/
        
        vector< vector<string> > results; results.resize(2);
        int countOpenBrace = 0;
        int countClosedBrace = 0;
        int openParen = 0;
        int closeParen = 0;
        string nextRow = "";
        bool end = false;
        allBlank = true;
        
        for (int i = 0; i < line.length(); i++) {
            
            if (m->control_pressed) { return results; }
            
            if (line[i] == '[')         { countOpenBrace++;     }
            else if (line[i] == ']')    { countClosedBrace++;   }
            else if (line[i] == '{')    { openParen++;          }
            else if (line[i] == '}')    { closeParen++;         }
            else if (openParen != 0)    { nextRow += line[i];   }  //you are reading the row info
            
            //you have reached the end of the rows info
            if ((countOpenBrace == countClosedBrace) && (countClosedBrace != 0)) { end = true; break; }
            if ((openParen == closeParen) && (closeParen != 0)) { //process row
                numRows++;
                
                vector<string> result = getNamesAndTaxonomies(nextRow);
                if (result.size() != 0) { results[0].push_back(result[0]); results[1].push_back(result[1]); if (result[1] != "") { allBlank = false; } }
                
                nextRow = "";
                openParen = 0;
                closeParen = 0;
            }
        }
        
        
        return results;
    }
    catch(exception& e) {
        m->errorOut(e, "BiomInfoCommand", "readRows");
        exit(1);
    }
}
//**********************************************************************************************************************
//items[0] = id, items[1] = taxonomy, if items[2] then thats the taxonomy bootstrap values
vector<string> BiomInfoCommand::getNamesAndTaxonomies(string line) {
    try {
        /*"rows":[
         {"id":"Otu01", "metadata":{"taxonomy":["Bacteria", "Bacteroidetes", "Bacteroidia", "Bacteroidales", "Porphyromonadaceae", "unclassified"], "bootstrap":[100, 100, 100, 100, 100, 100]}},
         {"id":"Otu02", "metadata":{"taxonomy":["Bacteria", "Bacteroidetes", "Bacteroidia", "Bacteroidales", "Rikenellaceae", "Alistipes"], "bootstrap":[100, 100, 100, 100, 100, 100]}},
         ...
         
         "rows":[{"id": "k__Archaea;p__Euryarchaeota;c__Methanobacteria;o__Methanobacteriales;f__Methanobacteriaceae", "metadata": null},
         {"id": "k__Bacteria;p__Actinobacteria;c__Actinobacteria;o__Actinomycetales;f__Actinomycetaceae", "metadata": null}
         ....
         
         make look like above
         
         
         ],*/
        
        vector<string> results;
        if (line == "") { return results; }
        
        int pos = line.find_first_of(',');
        if (pos == string::npos) { //some kind of error?? we expect at least metadata : null, just grab name
            results.push_back(getName(line)); results.push_back("");
        }else {
            string value;
            m->splitAtComma(value, line);  //value hold name portion ("id":"Otu01") line holds rest
            results.push_back(getName(value));
            
            string taxonomy = ""; string bootstrap = "";
            int pos = line.find("taxonomy");
            if (pos != string::npos) { //no taxonomy info given
                int pos2 = line.find("bootstrap");
                if (pos2 != string::npos) { //no taxonomy info given
                    taxonomy = line.substr(pos, (pos2-pos));
                    bootstrap = line.substr(pos2);
                }else {
                    taxonomy = line.substr(pos);
                }
            }
            
            results.push_back(getTaxonomy(taxonomy, bootstrap));
        }
        
        return results;
    }
    catch(exception& e) {
        m->errorOut(e, "BiomInfoCommand", "getNamesAndTaxonomies");
        exit(1);
    }
}
//**********************************************************************************************************************
string BiomInfoCommand::getName(string line) {
    try {
        vector<string> nameItems;
        m->splitAtChar(line, nameItems, ':'); //split part we want containing the ids
        string name = nameItems[1];
        
        //remove "" if needed
        int pos = name.find("\"");
        if (pos != string::npos) {
            string newName = "";
            for (int k = 0; k < name.length(); k++) {
                if (name[k] != '\"') { newName += name[k]; }
            }
            name = newName;
        }
        
        return name;
    }
    catch(exception& e) {
        m->errorOut(e, "BiomInfoCommand", "getName");
        exit(1);
    }
}
//**********************************************************************************************************************
//"taxonomy":"Bacteria", "Bacteroidetes", "Bacteroidia", "Bacteroidales", "Porphyromonadaceae", "unclassified",
//"bootstrap":100, 100, 100, 100, 100, 100
string BiomInfoCommand::getTaxonomy(string taxonomy, string bootstrap) {
    try {
        vector<string> results;
        
        if (taxonomy != "") {
            vector<string> taxItems;
            m->splitAtChar(taxonomy, taxItems, ':'); //split part we want containing the ids
            string taxons = taxItems[1];
            
            string taxon;
            while((taxons.find_first_of(',') != -1)) {
                if (m->control_pressed) {break;}
                m->splitAtComma(taxon, taxons);
                results.push_back(taxon);
            }
            if (taxons != "") { results.push_back(taxons); }
        }
        
        if (bootstrap != "") {
            vector<string> bootItems;
            m->splitAtChar(bootstrap, bootItems, ':'); //split part we want containing the ids
            string bootValues = bootItems[1];
            
            string bootValue;
            int i = 0;
            while((bootValues.find_first_of(',') != -1)) {
                if (m->control_pressed) {break;}
                m->splitAtComma(bootValue, bootValues);
                results[i]+="("+bootValue+")";
                i++;
            }
            if (bootValues != "") { results[i]+="("+bootValues+")"; }
        }
        
        string result = "";
        for (int i = 0; i < results.size(); i++) {
            if (m->control_pressed) {result = ""; break;}
            result += results[i] + ";";
        }
        
        return result;
    }
    catch(exception& e) {
        m->errorOut(e, "BiomInfoCommand", "getTaxonomy");
        exit(1);
    }
}
//**********************************************************************************************************************
//designed for things like "type": "OTU table", returns type
string BiomInfoCommand::getTag(string& line) {
    try {
        bool inQuotes = false;
        string tag = "";
        char c = '\"';
        
        for (int i = 0; i < line.length(); i++) {
            
            //you want to ignore any ; until you reach the next '
            if ((line[i] == c) && (!inQuotes)) {  inQuotes = true;  }
            else if ((line[i] == c) && (inQuotes)) {
                inQuotes= false;
                line = line.substr(i+1);
                return tag;
            }
            
            if (inQuotes) {  if (line[i] != c) { tag += line[i]; }  }
        }
        
        return tag;
    }
    catch(exception& e) {
        m->errorOut(e, "BiomInfoCommand", "getTag");
        exit(1);
    }
}
//**********************************************************************************************************************
void BiomInfoCommand::printSharedData(vector<SharedRAbundVector*> thislookup, ofstream& out) {
    try {
        
        m->clearGroups();
        map<string, SharedRAbundVector*> Ovectors;
        for (int i = 0; i < thislookup.size(); i++) { Ovectors[thislookup[i]->getGroup()] = thislookup[i]; }
        
        //initialize bin values
        for (map<string, SharedRAbundVector*>::iterator it = Ovectors.begin(); it != Ovectors.end(); it++) {
            out << (it->second)->getLabel() << '\t' << it->first << '\t';
            (it->second)->print(out);
                
            Groups.push_back(it->first);
        }
        m->setGroups(Groups);
        
    }
    catch(exception& e) {
        m->errorOut(e, "BiomInfoCommand", "printSharedData");
        exit(1);
    }
}

//**********************************************************************************************************************
