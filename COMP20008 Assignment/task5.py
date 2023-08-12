"""
COMP20008 Semester 1
Assignment 1 Task 5
"""
import numpy as np
import pandas as pd
from typing import Dict, Union, List

from matplotlib import pyplot as plt
from sklearn.feature_extraction.text import CountVectorizer
from sklearn.decomposition import PCA
from sklearn.preprocessing import Normalizer


def add_graph(df, ax, pc):
    # find the largest and smallest PC weights
    max_10 = df.nlargest(10, [pc])
    min_10 = df.nsmallest(10, [pc])

    # plot the graph
    ax.set(xlabel='Words', ylabel=pc)
    ax.bar(max_10.index.tolist(), max_10.max(axis='columns').values, label=f"Largest {pc}")
    ax.bar(min_10.index.tolist(), min_10.min(axis='columns').values, label=f"Smallest {pc}")
    ax.tick_params(axis='x', rotation=90)
    ax.set_title(f'Top 10 Largest & Smallest Weighted Words ({pc})')
    return ax


def get_dict(df, key):
    # find the largest and smallest PC weights
    max_10 = df.nlargest(10, [key])
    min_10 = df.nsmallest(10, [key])

    # return the required structure
    return {
        "positive": max_10.index.tolist(),
        "negative": min_10.index.tolist(),
        "positive_weights": max_10[key].tolist(),
        "negative_weights": sorted(min_10[key].tolist(), reverse=True),
    }


# Task 5 - Dimensionality Reduction (3 marks)
# some code referenced from week 6 workshop
def task5(bow_df: pd.DataFrame, tokens_plot_filename: str, distribution_plot_filename: str) -> Dict[str, Union[List[str], List[float]]]:
    # creating a bag of words representation
    vectorizer = CountVectorizer()
    bow = vectorizer.fit_transform(bow_df['words'])

    # normalisation using sklearn max
    normaliser = Normalizer(norm='max')
    bow_norm = normaliser.fit_transform(bow.toarray())

    # perform 2 component pca
    pca = PCA(n_components=2, random_state=535)
    x_pca = pca.fit_transform(bow_norm)

    # calculate variance for PC1 and PC2
    variance = pca.explained_variance_ratio_
    # find component explaining the greatest and smallest variance
    max_var, min_var = ['PC1', 'PC2'] if variance[0] > variance[1] else ['PC2', 'PC1']

    # insert the transposed components into a dataframe.
    weights = pd.DataFrame(pca.components_.T, columns=['PC1', 'PC2'])
    # the index of the dataframe are the word features.
    weights.index = vectorizer.get_feature_names_out()

    # Graph 5a, plotting bar graphs for top 10 largest / smallest weights.
    fig, ax = plt.subplots(2, 1, figsize=(13, 13))
    plt.subplots_adjust(hspace=0.3)  # to avoid x-axis label overlap
    ax[0] = add_graph(weights, ax[0], 'PC1')
    ax[1] = add_graph(weights, ax[1], 'PC2')
    plt.savefig(tokens_plot_filename)

    # Graph 5b, plotting a scatter graph with all seed urls.
    bow_df['pc1'], bow_df['pc2'] = x_pca[:, 0], x_pca[:, 1]
    plt.figure(figsize=(8, 6))
    # for each seed url, collect the values and plot the points.
    for label in np.unique(bow_df['seed_url'].values):
        match = bow_df[bow_df['seed_url'] == label]
        plt.scatter(match['pc1'].values, match['pc2'].values, label=label)
    plt.xlabel('PC1')
    plt.ylabel('PC2')
    plt.legend()
    plt.title('PCA Distribution of Seed Links')
    plt.savefig(distribution_plot_filename)

    return {"0": get_dict(weights, max_var), "1": get_dict(weights, min_var)}
