""" 
COMP20008 Semester 1
Assignment 1 Task 3
"""

from typing import Dict, List
import pandas as pd
from task2 import task2


# Task 3 - Producing a Bag Of Words for All Pages (2 Marks)
def task3(link_dictionary: Dict[str, List[str]], csv_filename: str):
    # list comprehension to build 'ans' array of dicts with output structure
    # by looping through all the links provided in link_dictionary
    ans = [{
            "link_url": link_url,
            "words": " ".join(task2(link_url, "task2_sample.json")),
            "seed_url": seed_url,
        }
        for seed_url in link_dictionary.keys()
        for link_url in link_dictionary[seed_url]]

    # sort 'ans' array by link_url
    ans = sorted(ans, key=lambda d: d["link_url"])

    # wrap our array in a dataframe and write to csv
    dataframe = pd.DataFrame(ans)
    dataframe.to_csv(csv_filename, index=False)

    return dataframe
