## Task 1: Get All Links (3 marks)
Guiding context: It may be useful for evaluating the quality and correctness of the process to initially examine the links which are in scope. Links excluded from the analysis will be those which will not be successfully retrieved or those which are beyond the scope of your task in evaluating the pipeline (e.g. links outside the current domain). In verifying your work, it is worth keeping in mind that errors earlier in the pipeline are more likely to lead to larger issues later in the pipeline.

Implement the function task1(..) in task1.py that takes two arguments:

The first argument is a list of starting links in the format of a list of strings and 

The second argument is a filename. 

Your function should crawl each starting link and output a json formatted output file with the filename given in second argument and should be in the following format:

1
{pageX: [linkXA, linkXB, ...], pageY: [linkYA, linkYB], ...}
where pageX and pageY  are the starting link strings, linkXA and linkXB are the pages found through crawling starting from pageX, and  linkYA and linkYB are pages found through crawling starting from pageY.

All URL strings should be fully qualified, including the protocol e.g. http://115.146.93.142/samplewiki/A12_scale should be used, even if the actual link found on the page is /samplewiki/A12_scale. Each webpage should be included in the list for each starting link only once and the list should be sorted alphabetically. You must crawl only links in the same domain as the starting link strings and should not include any links in the output which are not successfully retrieved.

You are free to write any helper functions (even in another file) if you like, but do not change the function definition of task1(..). For example, do not add any additional arguments to task1(..). This applies to all tasks.

To run your solution, open the terminal and run python main.py task1 full . You can verify your answer against the sample data using python main.py task1 sample, this will check the output against the sample data.

Note that for all tasks, the sample data verification is not intended to verify the correctness of your approach or the structure of your solution. Verifying your output and process are correct is part of all tasks, so you should treat it as a sanity check, ensuring you haven't missed any aspects of basic formatting. In all tasks, this sample used for verification should be considered arbitrarily selected, so there is no implicit guarantee or expectation that it will cover all cases or highlight issues present in the full scale task.

For performance reasons, your implementation of all tasks should cache (i.e. download once and keep in memory for the duration of the task) and respect the robots.txt on the site. For Task 2, your code will not be tested on URLs which are excluded under robots.txt, but you are certainly welcome to handle this behaviour there as well if you like.

## Task 2: Extracting Words from a Page (4 marks)
Guiding context: For this task, we are looking at a single page to see that we are successfully processing its content before we apply this process to all pages in later tasks. To make this as useful as possible for diagnosing issues which might affect later tasks, we will perform all steps in the pre-processing, this will include removing common elements not related to the content of the page (e.g. navigation menus and quality issues) and to remove low information words. The output is intended to be a list of words, useful to compare against the HTML page to see what has been removed and what has been retained.

As a warning, this kind of task is historically error prone, it is critically important to understand what you are doing at each and every step and to consider additional methods to verify your approach - any error in any step of your approach can lead to either subtle errors or major errors, both of which are unacceptable in good data science. At some point in your approach, it is strongly recommended you spend a bit of time thinking about and trying alternative approaches and that you investigate any discrepancies.

For this task you should use BeautifulSoup. Each page will contain a div with the id of mw-content-text , which processing will be restricted to. You can use inspect element tool on one of the pages (or through Wikipedia itself) to see what this typically covers.

Implement the function task2(..) in task2.py which takes a URL and a filename and outputs a json formatted file with that filename. The format of the JSON object output should be the name of the URL as the string and the value should be a list of words. If the page cannot be retrieved, the list should contain no values.

The page fetched should be encoded to its apparent_encoding attribute.

Your function should save its output to the filename given in the second argument.

Two stages of pre-processing should occur, the first narrowing down and removing all irrelevant elements from the page, and the second which produces word tokens and removes irrelevant tokens. The first stage comprises eight steps:

For the rest of the steps, operate on the div element with id of mw-content-text discarding other parts of the page.

From the remaining page tree, remove all th elements with the class of infobox-label .

From the remaining page tree, remove all div elements with the class of printfooter .

From the remaining page tree, remove all div elements with the id of toc .

From the remaining page tree, remove all table elements with the class of ambox.

From the remaining page tree, remove all div elements with the class of asbox.

From the remaining page tree, remove all span elements with the class of mw-editsection.

From the remaining mw-content-text tree, extract the text from the page using BeautifulSoup, this extraction should use a space separator (' ') between elements, and should not connect text from adjacent elements. For example if a section of the remaining tree contents were <p>a</p><p>b</p> it should become 'a b' , not 'ab'.

After the first stage's step 1 -- 8 have been completed, the remaining text should be converted to tokens in the following steps

Change all characters to their casefolded form and then normalize all page text to its NFKD form.

Convert all non-alphabetic characters (for example, numbers, apostrophes and punctuation), except for spacing characters (for example, whitespaces, tabs and newlines) and the backslash character ('\') to single-space characters. For example, '&' should be converted to ' '. You should consider non-English alphabetic characters as non-alphabetic for the purposes of this conversion.

Convert all spacing characters such as tabs and newlines into single-space characters, and ensure that only one whitespace character exists between each token.

The text should then be converted to explicit tokens. This process should use single-space characters as the boundary between tokens. 

Remove all stop words in nltk’s list of English stop words from the tokens in the page text.

Remove all remaining tokens that are less than two characters long from the page text.

Each token should then be converted to its Porter stemming algorithm stemmed form. This should use all the default improvements present in the NLTK PorterStemmer (including both the additional improvements beyond the original algorithm and NLTK's improvements).

Though you are expected to ensure and verify all steps are correctly carried out, if you decide to use library functions for step 4, it is critically important you carefully read and fully understand the documentation as failing to read documentation could lead to major errors in pre-processing steps.

Once steps 1 -- 7 are done, build a JSON file representing the page.  The JSON file should contain a JSON object containing one string matching the fully qualified url requested and the associated value should be an array of strings corresponding to the tokens produced by the above pre-processing.

If the page is not successfully retrieved, the array should contain no strings.

To run your solution, open the terminal and run python main.py task2 full - this runs the task against a different url to the sample data. You can verify your answer against the sample data using python main.py task2 sample, this will check the output against the sample data. For this task an additional option is provided, extra, by adding a link at the end of the task, you can view the output for a given URL, e.g. python main.py task2 extra http://115.146.93.142/samplewiki/Pythagorean_tuning .

## Task 3: Producing a Bag Of Words for All Pages (2 marks)
Guiding Context: For this task, we're looking at putting the first two tasks together to get all the words on each page from all pages to produce a representation of all our data which is easily processed later. This will also allow us to look for procedural errors in our text processing pipeline which may not have been present in our initial sample of the dataset.

Implement the function task3(..) in task3.py which takes a dictionary containing key/value pairs matching the output format of Task 1 and a filename and outputs a csv formatted file with that filename. 

For each key in the dictionary, all links should be visited. The key should be considered the seed_url and the link should be considered the link_url. The HTML page for each link_url should be retrieved and processed into a list of tokens according to both stages of pre-processing in Task 2, this list of tokens should then be joined with a single space and form the words string associated with the page. Once all pages are processed, a dataframe should be created which is output to a new csv file with the given filename.

The csv output should contain the headings link_url, words, seed_url and each row should contain the details of one page, with

link_url being the fully qualified URL of the page the bag of words corresponds to,

words being a string comprising all tokens for the link_url's page, separated by exactly one space character and

seed_url being the key which the link URL was taken from in the dictionary argument.

If no tokens are retrieved after pre-processing, the words string should be an empty string ''.

The rows in the csv file should be in ascending order of link_url and then (where the link_url is the same for two or more pages) seed_url.

To run your solution, open the terminal and run python main.py task3 full - this will first run task1 and then use the output of task1 as input to task3. You can verify your answer against the sample data using python main.py task3 sample, this will check the output against the provided sample data.

## Task 4: Plotting the Most Common Words (2 marks)
Guiding Context: For this task, we're interested in what common words are present in each of the seed URL's crawls. We'd like to know whether our pre-processing so far is effective in removing words which aren't meaningful and to get a very general feel for how much overlap there is in the two vocabularies.

Implement the function task4(..) in task4.py which takes a pandas dataframe matching the output format of Task 3 and one filename and outputs a plot to that filename. task4(..) should return a dictionary where each seed_url is a key and the value is the list of 10 most common words.

In this task you should generate a plot which should allow the comparison of the top 10 most common words in each seed_url . Here the metric of number of total occurrences across all pages should be used (e.g. if one page has two uses of a word, both should be counted in the total for the seed_url). For consistency in your output, if multiple words appear equally frequently, they should appear in alphabetical order in your plot.

To run your solution, open the terminal and run python main.py task4 full . To verify the top 10 words for the sample dataset, use python main.py task4 sample .

## Task 5: Dimensionality Reduction (3 marks)
Guiding Context: Task 4 provided some general insight into what words are often present in each general topic. This gives us a bit of an idea of what we might want to look for, but if we want to work out which topic a particular page is about, this alone might not be the most helpful tool! In this task we're interested in visualising the high dimensionality space produced by the bag of words by reducing it down to two components to plot. This is another step in verifying our results and determining whether our approach appears sensible, as with all tasks, it may be useful to look at the question you are trying to answer. This allows us to determine whether the processing has clearly separated the data or whether there's a high degree of ambiguity.

Implement the function task5(..) in task5.py which takes one pandas dataframe matching the output format of Task 3 and two filenames and outputs a plot to each of the given filenames.

In task5(..) you should first produce a bag of words representation of the words across all pages and then perform normalisation using sklearn's max norm fit to the dataframe, following this, you should perform Principal Component Analysis using 2 components to the normalised dataframe.

task5() should output two files:

The plot output to the first file should show the top 10 most positively weighted tokens and their weights and 10 most negatively weighted tokens and their weights for each component.

The plot output to the second file should show where articles from each seed_url fall on the 2-component axes.

task5() should return a dictionary where the PCA component ID is the key (0 for the component explaining the greatest variance and 1 for the component explaining the next greatest variance) and the value is also a dictionary, where this nested dictionary has four keys, "positive", "negative", "positive_weights", "negative_weights" and the associated value of each key is as follows:

The value for the positive key should be a list of the top 10 tokens with highest magnitude positive weights.

The value for the negative key should be a list of the top 10 tokens with highest magnitude negative weights.

The value for the positive_weights key should be the weight in the PCA decomposition of the relevant token in the positive token list.

The value for the negative_weights key should be the weight in the PCA decomposition of the relevant token in the negative token list.

Where a random_state option is provided, you should use 535.

To run your solution, open the terminal and run python main.py task5 full . To verify the top 10 PCA components for the sample dataset, use python main.py task5 sample .

Note that minor numerical errors could produce variation in your results, but non-numerical errors can also produce variation in your results - these two factors mean that as with all sample dataset verification you should not blindly trust your results are correct, but instead ensure you verify you are following the correct process.