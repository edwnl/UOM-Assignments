-- Your Name: Edwin Li
-- Your Student Number: 1383154
-- By submitting, you declare that this work was completed entirely by yourself.
-- Note the RANK() function is used for questions to find MAX rows. 
-- It avoids the need of a subquery, and is referenced from 
stackoverflow.com/a/37780458
-- 
___________________________________________________________________________________
___________________________________________________________________________________
______________________________________
-- BEGIN Q1
SELECT id, title FROM publication WHERE id NOT IN
(SELECT referencingPublicationId FROM referencing); 
-- END Q1
-- 
___________________________________________________________________________________
___________________________________________________________________________________
______________________________________
-- BEGIN Q2
SELECT id, title, dateOfPublication FROM publication 
WHERE dateOfPublication = (SELECT MAX(dateOfPublication) FROM publication);
-- END Q2
-- 
___________________________________________________________________________________
___________________________________________________________________________________
______________________________________
-- BEGIN Q3
SELECT p.id, p.title FROM publication p
INNER JOIN coauthors ca ON p.id = ca.publicationId AND ca.AuthorID = (
SELECT id from researcher r 
WHERE r.firstName = 'Renata' AND r.lastname = 'Borovica-Gajic'
)
WHERE p.endPage - p.startPage >= 10;
/*
Papers from Renata: 1, 2, 3, 5, 9
Page Length For the above respectively: 10, 15, 4, 11, 19
Answer: 1, 2, 5, 9
*/
-- END Q3
-- 
___________________________________________________________________________________
___________________________________________________________________________________
______________________________________
-- BEGIN Q4
SELECT id, title, citationCount FROM (
SELECT p.id, p.title, COUNT(r.referencingPublicationId) as citationCount,
RANK() OVER (ORDER BY COUNT(r.referencingPublicationId) DESC) AS pos 
FROM publication p
 LEFT JOIN referencing r ON p.id = r.referencedPublicationId GROUP BY p.id
) derived_pos_table
WHERE pos = 1;
/* 
Survey on databases 6
Survey on machine learning 3
Indexing in databases 4
Efficiency of queries 2
Efficiency of indexes 1
Segmenting text 3
Inferring paragraph breaks 2
A survey on natural language processing 0
Learning indexing 2
User evaluation 3
User evaluation 0
Cyber security survey 1
Cyber security 101 0
Answer: Survey on databases 6
*/
-- END Q4
-- 
___________________________________________________________________________________
___________________________________________________________________________________
______________________________________
-- BEGIN Q5
SELECT p.id AS publicationID, p.documentURL FROM publication p
INNER JOIN publication_has_keyword pk ON p.id = pk.publicationId
AND pk.keywordID = (SELECT id from keyword where word = 'Databases')
WHERE p.id IN (SELECT referencedPublicationId from referencing);
/* 
Database Publications: 1, 3, 4, 5
Cited At least once: 1-7, 9, 10, 12
Answer: 1, 3, 4, 5
*/
-- END Q5
-- 
___________________________________________________________________________________
___________________________________________________________________________________
______________________________________
-- BEGIN Q6
SELECT r.firstName, r.lastName FROM researcher r
-- Find all authors which have appeared in the top ten list at least once.
INNER JOIN coauthors ca ON r.id = ca.authorId AND r.id IN (
SELECT authorID from coauthors ca 
WHERE ca.PublicationID IN (SELECT publicationId from topten)
)
LEFT JOIN referencing rf ON referencedPublicationId = ca.publicationId 
GROUP BY r.id HAVING COUNT(referencedPublicationId) >= 2;
/* 
>= 2 Citations Overall: Renata, Farhana, Colton, Tim, Ling, Jane, Layla, Kara
>= 1 Top Ten Apperance: Renata, Farhana, Colton, Tim, Ling, Jane, Layla, Kara
Answer: Renata, Farhana, Colton, Tim, Ling, Jane, Layla, Kara
*/
-- END Q6
-- 
___________________________________________________________________________________
___________________________________________________________________________________
______________________________________
-- BEGIN Q7
SELECT id, word FROM (
SELECT k.id, k.word, COUNT(DISTINCT t.publicationId) as apperanceCount,
RANK() OVER (ORDER BY COUNT(DISTINCT t.publicationId) DESC) AS pos FROM
keyword k
INNER JOIN publication_has_keyword pk ON k.id = pk.keywordId
INNER JOIN topten t ON pk.publicationId = t.publicationId
GROUP BY k.id 
) derived_pos_table
WHERE pos = 1;
/*
Areas and the Publications in TopTen
Database 3
Machine Learning 2
Natural language processing 2
Cyber security 2
Human-computer interaction 1
Answer: 
1 Database
*/
-- END Q7
-- 
___________________________________________________________________________________
___________________________________________________________________________________
______________________________________
-- BEGIN Q8
SELECT firstName, lastName, totalCitationCount
FROM (
 SELECT r.firstName, r.lastName, COUNT(referencedPublicationId) as 
totalCitationCount, 
RANK() OVER (ORDER BY COUNT(referencedPublicationId) DESC) AS pos FROM 
researcher r
 INNER JOIN coauthors author ON r.id = author.authorId
 INNER JOIN publication p ON author.publicationId = p.id
 INNER JOIN referencing ref ON referencedPublicationId = p.id
 GROUP BY r.id
) derived_pos_table
WHERE pos = 1;
/* 
Total Citations:
Renata 16
Colton 14
Farhana 9
Jane 9
Tim 7
Ling 4
Layla 3
Kara 2
Answer: Renata - 16
*/
-- END Q8
-- 
___________________________________________________________________________________
___________________________________________________________________________________
______________________________________
-- BEGIN Q9
SELECT DISTINCT r.firstName, r.lastName from researcher r
INNER JOIN coauthors author on r.id = author.authorId
INNER JOIN publication p ON author.publicationId = p.id
-- For each publication, JOIN all the authors which collaborated.
INNER JOIN coauthors collaborator ON p.id = collaborator.publicationId 
AND collaborator.AuthorID != author.authorID -- Not collaborating if it's 
with themselves
-- AUTHOR is from Databases
INNER JOIN researcher_has_keyword auth_key ON author.authorId = 
auth_key.researcherId 
AND auth_key.keywordID = (SELECT id from keyword WHERE word = 'Databases')
-- CO-AUTHOR is from Machine Learning 
INNER JOIN researcher_has_keyword collab_key ON collaborator.authorId = 
collab_key.researcherId 
AND collab_key.keywordId = (SELECT id from keyword WHERE word = 'Machine 
Learning');
/*
Database Researchers: Renata, Farhana, Jane
Machine Learning Researchers: Renata, Colton, Kara, Layla
Situations in which the Collab under Q9's Rules (DB_AUTHOR - ML_COLLABORATOR)
Pub 1: Renata - Colton, Farhana - Renata, Farhana - Colton
Pub 2: Renata - Colton, Renata - Layla
Pub 3: Jane - Renata
Pub 4: Farhana - Kara, Jane - Kara
Pub 5: Farhana - Renata
Pub 9: Jane - Kara
Answer: Renata, Farhana, Jane
*/
-- END Q9
-- 
___________________________________________________________________________________
___________________________________________________________________________________
______________________________________
-- BEGIN Q10
SELECT r.firstName, r.lastName FROM researcher r
-- Researchers who HAVE NOT co-authed with Renata AFTER 1/1/23
WHERE r.id NOT IN (
 SELECT DISTINCT ca.authorId FROM coauthors ca
 INNER JOIN publication p ON ca.publicationId = p.id
 INNER JOIN coauthors collaborator ON p.id = collaborator.publicationId 
AND collaborator.AuthorID != ca.authorId -- Not collaborating if it's with 
themselves
AND collaborator.AuthorID = (SELECT id FROM researcher WHERE firstName = 
'Renata' AND lastName = 'Borovica-Gajic')
 WHERE p.dateOfPublication >= '2023-01-01'
)
-- Researchers who HAVE co-authed with Renata BEFORE 1/1/23
AND r.id IN (
 SELECT DISTINCT ca.authorId FROM coauthors ca
 INNER JOIN publication p ON ca.publicationId = p.id
 INNER JOIN coauthors collaborator ON p.id = collaborator.publicationId 
AND collaborator.AuthorID != ca.authorId -- Not collaborating if it's with 
themselves
AND collaborator.AuthorID = (SELECT id FROM researcher WHERE firstName = 
'Renata' AND lastName = 'Borovica-Gajic')
 WHERE p.dateOfPublication < '2023-01-01'
);
/*
Have Co-authed after 1/1/23 with Renata: NONE
Have co-authed before 1/1/23 with Renata: Farhana, Colton, Tim, Ling, Jane, Layla 
(2, 3, 4, 5, 6, 8)
Answer is the intersection of the above sets.
Answer: Farhana, Jane, Colton, Tim, Ling, Layla
*/
-- END Q10
-- 
___________________________________________________________________________________
___________________________________________________________________________________
______________________________________
-- END OF ASSIGNMENT Do not write below this lin