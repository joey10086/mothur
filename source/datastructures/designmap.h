//
//  designmap.h
//  Mothur
//
//  Created by SarahsWork on 6/17/13.
//  Copyright (c) 2013 Schloss Lab. All rights reserved.
//

#ifndef __Mothur__designmap__
#define __Mothur__designmap__

#include "mothurout.h"

/* This class is a representation of the design file.  
 
 group    treatment    sex        age
 F000142    Early       female    young
 F000132    Late        male        old
 F000138    Mid         male        old
 
 
 */

class DesignMap {
public:
	DesignMap() { m = MothurOut::getInstance(); defaultClass = "not found"; }
	DesignMap(string); 
	~DesignMap() {}
    
    int read(string);
    string get(string, string); //groupName, category returns value. example F000132, sex -> male
    string getGroup(string); //groupName, returns first categories value. example F000132, -> late
    vector<string> getCategoryValues(string); //categoryName, returns values. example treatment, -> early,late,mid
	int set(string, map<string, string>); //groupName, map<category, value>
    int push_back(string, vector<string>); //groupName, vector<value> - assumes you put values in order of getNamesOfCategories
	vector<string> getNamesOfCategories() {
		sort(namesOfCategories.begin(), namesOfCategories.end());
        return namesOfCategories;
	}
    int getNumCategories() { return namesOfCategories.size(); }
	int getNum()  {  return designMap.size();  }
    int getNumUnique(map<string, vector<string> >); //get number of groups belonging to a category or set of categories, with value or a set of values. Must have all categories and values. Example:
    //  map<treatment - > early, late>, <sex -> male> would return 1. Only one group is male and from early or late.
    int getNumShared(map<string, vector<string> >); //get number of groups belonging to a category or set of categories, with value or a set of values. Must have at least one categories and values. Example:
    //  map<treatment - > early, late>, <sex -> male> would return 3. All three group have are either male or from early or late.

    vector<string> getNamesOfGroups() { return groups; }
    vector<string> getNames(string, string); //get names groups with category and value.
	vector<string> getNamesUnique(map<string, vector<string> >); //get names of groups belonging to a category or set of categories, with value or a set of values. Must have all categories and values. Example:
    //  map<treatment - > early, late>, <sex -> male> would return F000132. F000132 is the only group which is male and from early or late.
    vector<string> getNamesShared(map<string, vector<string> >); //get names of groups belonging to a category or set of categories, with value or a set of values. Must have at least one categories and values. Example:
    //  map<treatment - > early, late>, <sex -> male> would return F000132, F000142, F000138. All three group have are either male or from early or late.

    int print(ofstream&);
    int printCategories(ofstream&, vector<string>); //print certain categories
    int printGroups(ofstream&, vector<string>); //print certain Groups
    
    string getDefaultClass() { return defaultClass; }
    
private:
    string defaultClass;
	MothurOut* m;
    vector< map<string, int> > totalCategories;  //for each category, total groups assigned to it.   vector[0] early -> 1, vector[1] male -> 2
    vector<string> groups;
    vector<string> namesOfCategories;
    vector< vector<string> > designMap;
    map<string, int> indexGroupNameMap; //maps groupName to row in values
    map<string, int> indexCategoryMap;  //maps category to column in values
};


#endif /* defined(__Mothur__designmap__) */
