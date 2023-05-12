
class DataStore:

    def __init__(self, file_them, file_3pre, file_3sub, file_3suf, file_4pre, file_4sub, file_4suf, file_5sub):
        self.prefix3 = self.read_substr_list(file_3pre)
        self.substr3 = self.read_substr_list(file_3sub)
        self.suffix3 = self.read_substr_list(file_3suf)
        self.prefix4 = self.read_substr_list(file_4pre)
        self.substr4 = self.read_substr_list(file_4sub)
        self.suffix4 = self.read_substr_list(file_4suf)
        self.substr5 = self.read_substr_list(file_5sub)
        self.them_dict = self.read_them_dic(file_them)

    def read_them_dic(self, filename):
        """Reads dictionary

        Args:
            filename ([string]): [dictionary file name]

        Returns:
            [list]: [list of words, with @ at both ends of each word]
        """
        f = open(filename, "r")
        result = []
        for x in f:
            result.append("@" + x.strip() + "@")
        f.close()
        return result

    def read_substr_list(self, filename):
        """Reads lines from a file and stores each line as a record in a list.
           Each line should be one single word

        Args:
            filename (string): path to input file

        Returns:
            list: list of words
        """
        f = open(filename, "r")
        result = []
        for x in f:
            result.append(x.strip())
        f.close()
        return result
