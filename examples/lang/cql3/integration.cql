CREATE TABLE race_winners (
   race_name ascii(15),
   race_position int,
   cyclist_name ascii(5),
   PRIMARY KEY (race_name, race_position)
);

INSERT INTO race_winners (race_name, race_position, cyclist_name)
  VALUES ('Graduation Race', 1, 'Alice')
  USING TTL 86400 AND TIMESTAMP 123456789;

SELECT race_name, race_position, cyclist_name
FROM race_winners
WHERE race_position=1;
