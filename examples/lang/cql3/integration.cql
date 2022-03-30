CREATE TABLE race_winners ( race_name ascii(19), race_position int, cyclist_name ascii(9), PRIMARY KEY (race_name, race_position));

INSERT INTO race_winners (race_position, race_name, cyclist_name) VALUES ( 'Graduation Race 1', 1, 'Alice 1') USING TTL 86400 AND TIMESTAMP 123456789;

INSERT INTO race_winners (race_position, race_name, cyclist_name) VALUES ( 'Graduation Race 2', 2, 'Alice 2') USING TTL 86400 AND TIMESTAMP 123456789;

INSERT INTO race_winners (race_position, race_name, cyclist_name) VALUES ( 'Graduation Race 3', 3, 'Alice 3') USING TTL 86400 AND TIMESTAMP 123456789;

INSERT INTO race_winners (race_position, race_name, cyclist_name) VALUES ( 'Graduation Race 4', 4, 'Alice 4') USING TTL 86400 AND TIMESTAMP 123456789;

SELECT race_name, race_position, cyclist_name FROM race_winners WHERE race_position = 1;

SELECT race_name, race_position, cyclist_name FROM race_winners WHERE race_position = 2;

SELECT race_name, race_position, cyclist_name FROM race_winners WHERE race_position = 3;

SELECT race_name, race_position, cyclist_name FROM race_winners WHERE race_position = 4;
