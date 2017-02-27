import re
import sys
import os
from os import walk


class Comment(object):
    
    def __init__( self, orginal_text):
        self.orginal_text = orginal_text
        self.new_text = ""

    def has_changed(self):
        return not self.orginal_text == self.new_text

    def replace(self, file_text):
        """
        Replaces the orginal text by the new text
        or if there is no orginal text inserts it at the beginning
        """
        if self.has_changed():
            if not self.orginal_text == "":
                return file_text.replace(self.orginal_text, self.new_text)
            else:
                return self.new_text + "\n" + file_text
        else:
            return file_text

    def update(self):
        """
        Applies updates to the comment
        Will be overwritten by subclass
        """
        self.new_text = self.orginal_text

    def __str__( self):
        return self.orginal_text + "\n->\n" + self.new_text

class IHC(Comment):
    """
    Information Head Comment
    is the first Comment and looks like:
    /*******
     * bla *
     ******/
    """
    def __init__(self, orginal_text):
        super().__init__(orginal_text)

    @staticmethod
    def is_IHC(comment_text):
        return comment_text.startswith("/**********") and comment_text.endswith("**********/")

    def update(self):
        """
        Applies updates to the comment
        """
        self.new_text = open("./comment_styles/IHC.txt", "r").read()

    def __str__(self):
        return "IHC: \n" + super().__str__()


class DHC(Comment):
    """
    Doxygen Head Comment
    is the second Comment looking like:
    /**
     * bla
     */
    """
    def __init__(self, orginal_text, filename):
        super().__init__(orginal_text)
        self.filename = filename
        self.author = "TODO"
        self.description = "TODO"
        self.names = {
                "olga" : "Olga Dedi",
                "dedi" : "Olga Dedi",
                "andreas" : "Andreas Zoor",
                "kai" : "Kai Beckmann",
                "kevin" : "Kevin Sapper",
                "steffen" : "Steffen Reichmann",
                }

    @staticmethod
    def is_DHC(comment_text):
        return comment_text.startswith("/**") and comment_text.endswith("*/")

    def get_info_from_old_comment(self, old_comment):
        result = False #true if comment was used to delete it
        lines = old_comment.split("\n")
        lines = lines[1:-1]
        new_lines = []
        for line in lines:
            new_lines.extend(line.split("*"))

        new_lines[:] = [s.strip() for s in new_lines] # delete all whitespace from beginning or ending 
        new_lines =  remove_values_from_list(new_lines, "") # removes all empty strings

        for line in new_lines:
            if line.startswith("Author:"):
                self.author = line.replace("Author:", "").strip()
                #replace with name
                for sname, lname in self.names.items():
                    if sname in self.author:
                        self.author = lname
                result = True
            if line.startswith("Description:"):
                self.description = line.replace("Description:", "").strip()
                result = True
            
        return result

    def update(self):
        """
        Applies updates to the comment
        """
        self.new_text = open("./comment_styles/DHC.txt", "r").read()
        self.new_text = self.new_text.replace("<file>", self.filename)
        self.new_text = self.new_text.replace("<author>", self.author)
        self.new_text = self.new_text.replace("<description>", self.description)

    def __str__(self):
        return "DHC: \n" + super().__str__()


def remove_values_from_list(the_list, val):
   return [value for value in the_list if value != val]

def head_comment(file_path):
    comments = []
    ihc = None
    dhc = None
    first_comment = "" 

    backup_path = os.path.join(os.path.split(file_path)[0], "org", os.path.split(file_path)[1])

    f = open(file_path,"r")
    f_backup = open(backup_path, "w")
    
    file_text = f.read()
    f_backup.write(file_text)

    f.close()
    f_backup.close()

    f_new = open(file_path,"w")

    #test if there is IHC and DHC at the beginning
    regex_IHC_DHC = r"\A[\s\n]*/\*\*\*\*(.|\n)*?\*\*\*\*/[\s\n]*/\*\*(.|\n)*?\*/"
    beginn_with = re.match(regex_IHC_DHC, file_text) != None

    # for all "/* */" comments
    regex = r'/\*(.|\n)*?\*/' # gets a "/* */" comments
    matches = re.finditer(regex, file_text)
    for matchNum, match in enumerate(matches):
        #get comment
        comment_text = match.group()
        #If in the file are already correct comments
        if beginn_with:
            if matchNum == 0:
                ihc = IHC(comment_text)
            elif matchNum == 1:
                dhc = DHC(comment_text, os.path.split(file_path)[1])
            elif matchNum == 2:
                first_comment = comment_text
        else:
            if matchNum == 0:
                first_comment = comment_text

    #print (first_comment)
    if ihc == None:
        ihc = IHC("")
    if dhc == None:
        dhc = DHC("", os.path.split(file_path)[1])
    
    if first_comment != "":
        delete_comment = dhc.get_info_from_old_comment(first_comment)
        # if the comment was used delete the comment from the file
        if delete_comment:
            file_text = file_text.replace(first_comment, "")

    for c in [dhc, ihc]:
        c.update()
        file_text = c.replace(file_text)
        #print(str(c))

    f_new.write(file_text)

def main():

    if(len(sys.argv) < 2):
        print("Too few args")
        exit(-1)
    path = sys.argv[1]
    print("Starting styler on: " + str(path))
    if not os.path.exists(path):
        print("Path does not exits: " + str(path))
        exit(-1)
    files = []
    for (dirpath, dirnames, filenames) in walk(path):
        files.extend(filenames)

    for f in files:
        head_comment(path + f)

if __name__ == "__main__":
    main()



