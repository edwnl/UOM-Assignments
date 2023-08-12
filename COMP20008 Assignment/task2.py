"""
COMP20008 Semester 1
Assignment 1 Task 2
"""

import json
import requests
import unicodedata
import re
from nltk.corpus import stopwords
from nltk.stem.porter import PorterStemmer
from bs4 import BeautifulSoup

STOP_WORDS = set(stopwords.words('english'))  # nltk stopwords
PORTER_STEMMER = PorterStemmer()  # stemmer used


# filters out unwanted tags, returns True if unwanted.
def tag_filter(tag):
    attr = tag.attrs
    class_ = attr['class'] if 'class' in attr else []
    id_ = attr['id'] if 'id' in attr else []

    match tag.name:
        case 'th':
            return 'infobox-label' in class_
        case 'div':
            return any(x in ['printfooter', 'asbox'] for x in class_) or 'toc' in id_
        case 'table':
            return 'ambox' in class_
        case 'span':
            return 'mw-editsection' in class_

    return False


# pre-processes a string based on steps 1-7
def text_preprocess(text):
    # 1. Text to NFKD, casefold characters.
    text = unicodedata.normalize('NFKD', text.casefold())
    # 2. Non-alphabetic characters to single space char.
    text = re.sub(r"[^a-z\s\\]+", " ", text)
    # 3. Matches all white space chars, replace with single spaced char.
    text = re.sub(r"\s+", " ", text)
    # 4. Tokenize the words
    tokens = text.split(' ')
    # 5 - 6. Remove all stop words, and all tokens < 2 char.
    tokens = [w for w in tokens if w not in STOP_WORDS and len(w) >= 2]
    # 7. Converting tokens to stemmed form
    return [PORTER_STEMMER.stem(w) for w in tokens]


# Task 2 - Extracting Words from a Page (4 Marks)
def task2(link_to_extract: str, json_filename: str):
    # ensuring the link is a valid link.
    page = requests.get(link_to_extract)
    if not page.ok:
        return
    soup = BeautifulSoup(page.content, 'html.parser')

    # using BeautifulSoup, find the target div. (step 1)
    content = soup.find('div', {"id": 'mw-content-text'})

    # complete steps 2-7, removing unwanted tags
    [tag.decompose() for tag in content.find_all(tag_filter)]

    # strip the strings from the tags, and combine into one string
    content = ' '.join([s for s in content.stripped_strings])

    # complete the pre-processing steps 1-7
    content = text_preprocess(content)

    # output the result to a JSON file
    with open(json_filename, "w") as file:
        json.dump({link_to_extract: content}, file)

    return content
