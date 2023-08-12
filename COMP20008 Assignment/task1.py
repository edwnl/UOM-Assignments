"""
COMP20008 Semester 1
Assignment 1 Task 1
"""

import json
import requests
import re
from typing import Dict, List
from bs4 import BeautifulSoup
from urllib.parse import urljoin
from robots import process_robots, check_link_ok

# matches exactly one `/` character
# does not match any link starting with `//`
# does not match any link ending in a file extension
# does not match any string containing `:`
HREF_PATTERN = re.compile(r"^\/(?!\/)(?!.*\.\w+$)(?!.*:).*$")

# assuming both domain and robot rules are static.
DOMAIN = 'http://115.146.93.142' 
ROBOT_RULES = process_robots(requests.get(f"{DOMAIN}/robots.txt").text)
SESSION = requests.session()  # used to make multiple requests faster
SAFE_PAGE_LIMIT = 1000  # safe limit to avoid mistakes


def task1(starting_links: List[str], json_filename: str) -> Dict[str, List[str]]:
    answer = {}

    for starting_link in starting_links:
        # using sets as the "in" lookup is faster for sets
        links = {starting_link}
        visited = set()

        # loop until no links are left, or SAFE_PAGE_LIMIT exceeded
        while links and len(visited) < SAFE_PAGE_LIMIT:
            link = links.pop()
            page = SESSION.get(link)
            # ignore any links that lead to 404 pages
            if not page.ok:
                continue

            soup = BeautifulSoup(page.text, 'html.parser')
            visited.add(link)  # track links so they are not visited twice

            # find all href links that:
            # 1. match HREF_PATTERN
            # 2. passes ROBOT_RULES
            # 3. is not visited already
            # add to links to visit in a future iteration.
            for tag in soup.findAll('a', href=HREF_PATTERN):
                link = urljoin(DOMAIN, tag['href'])
                if not check_link_ok(ROBOT_RULES, tag['href']):
                    continue
                if link in visited:
                    continue
                links.add(link)

        # sort links
        answer[starting_link] = sorted(visited)

    with open(json_filename, "w") as f:
        json.dump(answer, f)

    return answer
