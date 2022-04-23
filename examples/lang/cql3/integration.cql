CREATE TABLE race_winners (
   race_name ascii(17),
   race_position int,
   cyclist_name ascii(7),
   PRIMARY KEY (race_name, race_position)
);

INSERT INTO race_winners (race_name, race_position, cyclist_name)
  VALUES ('Graduation Race', 1, 'Alice')
  USING TTL 86400 AND TIMESTAMP 123456789;

INSERT INTO race_winners (race_name, race_position, cyclist_name)
  VALUES ('Grasdation Race', 2, 'Asdce')
  USING TTL 86400 AND TIMESTAMP 1234226789;


DROP TABLE race_winners;

