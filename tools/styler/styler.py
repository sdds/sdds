import re
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
    def __init__(self, orginal_text):
        super().__init__(orginal_text)

    @staticmethod
    def is_DHC(comment_text):
        return comment_text.startswith("/**") and comment_text.endswith("*/")

    def __str__(self):
        return "DHC: \n" + super().__str__()


def remove_values_from_list(the_list, val):
   return [value for value in the_list if value != val]

def prepare_doxy_com(string):
    lines = string.split("\n")
    lines = lines[1:-1]
    new_lines = []
    for line in lines:
        new_lines.extend(line.split("*"))

    new_lines[:] = [s.strip() for s in new_lines] # delete all whitespace from beginning or ending 
    new_lines =  remove_values_from_list(new_lines, "") # removes all empty strings
    return new_lines

def head_comment(file_path):
    comments = []
    ihc = None
    dhc = None

    f = open(file_path,"r")
    f_backup = open(file_path + ".old", "w")
    f_new = open(file_path + ".new", "w")
    
    file_text = f.read()
    f_backup.write(file_text)

    regex = r'/\*(.|\n)*?\*/' # gets a "/* */" comments
    matches = re.finditer(regex, file_text)
    for matchNum, match in enumerate(matches):
        comment_text = match.group()
        if IHC.is_IHC(comment_text):
            ihc = IHC(comment_text)
        elif DHC.is_DHC(comment_text):
            dhc = DHC(comment_text)

    if ihc == None:
        ihc = IHC("")
    if dhc == None:
        dhc = DHC("")

    for c in [ihc, dhc]:
        c.update()
        file_text = c.replace(file_text)
        #print(str(c))

    f_new.write(file_text)

def main():
    path = "./test/"
    files = []
    for (dirpath, dirnames, filenames) in walk(path):
        files.extend(filenames)

    for f in files:
        head_comment(path + f)

if __name__ == "__main__":
    main()



