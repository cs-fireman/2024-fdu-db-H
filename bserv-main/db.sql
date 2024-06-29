CREATE TABLE auth_user (
    id serial PRIMARY KEY,
    username character varying(255) NOT NULL UNIQUE,
    password character varying(255) NOT NULL,
    is_superuser boolean NOT NULL,
    first_name character varying(255) NOT NULL,
    last_name character varying(255) NOT NULL,
    email character varying(255) NOT NULL,
    is_active boolean NOT NULL
);
CREATE TABLE xx (
    note_id SERIAL PRIMARY KEY,
    user_id INT REFERENCES auth_user(user_id),
    note_content TEXT NOT NULL
);

