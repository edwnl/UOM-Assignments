"""
COMP20008 Semester 1
Assignment 1 Task 4
"""

import matplotlib.pyplot as plt
import pandas as pd
from typing import List, Dict
from collections import Counter


# Function to count the words in a string
def count_words(text):
    return Counter(text.split()).most_common(10)


# Task 4 - Plotting the Most Common Words (2 Marks)
def task4(df: pd.DataFrame, output_plot_filename: str) -> Dict[str, List[str]]:
    ans = {}
    # group df by "seed_url" and add all the words to one string
    df = df.groupby("seed_url")['words'].agg(' '.join).reset_index()

    # array of tuples containing the 10 most common words
    most_common = df['words'].apply(count_words)

    # append the word count data as a column to the dataframe
    df['word_counts'] = most_common

    # plotting the graph
    fig, ax = plt.subplots(figsize=(10, 10))
    fig.suptitle("Word Frequency Comparison")
    ax.set(xlabel='Word', ylabel='Frequency')

    # for each row in the dataframe
    for row in df.itertuples():
        seed_url = row[1]
        # returns an array of words and frequencies from the tuple.
        words, freq = zip(*row[3])
        # bar graph with x=words, and y=freq, labelled by seed_url
        ax.bar(words, freq, label=seed_url)
        # construct the required dict to return
        ans[seed_url] = words

    ax.legend()
    # this is so the word labels are readable
    plt.xticks(rotation=90)
    # save the graph to the target location
    plt.savefig(output_plot_filename)

    return ans
