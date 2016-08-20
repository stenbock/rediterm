#include "parser.h"

int get_string_jobj(json_object *jobj, const char *str, char **dest_str)
{
	size_t len;
	json_object *jstring;

	/* Get title */
	json_object_object_get_ex(jobj, str, &jstring);
	len=strlen(json_object_get_string(jstring));
	if(len < 1)
		return 1;
	*dest_str = malloc(sizeof(char) * (len + 1));
	strcpy(*dest_str, json_object_get_string(jstring));

	return 0;

}

void parse_post_idx(post *p, json_object *jobj)
{
	get_string_jobj(jobj, "title", &p->title);
	get_string_jobj(jobj, "author", &p->author);
	get_string_jobj(jobj, "url", &p->url);
	get_string_jobj(jobj, "permalink", &p->permalink);
}

int sub_parse(char *str, post **post_list)
{
	json_object *jobj = json_tokener_parse(str);
	json_object *data;

	json_object *array;
	int array_len;
	json_object *post_json;
	json_object *post_json_data;
	post *parsed_post;
	post *parsed_post_prev;
	int i;

	if (json_object_object_get_ex(jobj, "data", &data) == 0)
		return 1;


	if (json_object_object_get_ex(data, "children", &array) == 0)
		return 2;
	
	array_len = json_object_array_length(array);
	for(i=0; i < array_len; i++) {
		parsed_post = malloc(sizeof(post));
		if (i == 0) {
			//printf("hello setting post_list\n");
			*post_list = parsed_post;
			parsed_post->head = NULL;
		}

		post_json = json_object_array_get_idx(array, i);
		json_object_object_get_ex(post_json, "data", &post_json_data);
		parse_post_idx(parsed_post, post_json_data);

		if (i != 0) {
			parsed_post_prev->tail = parsed_post;
			parsed_post->head = parsed_post_prev;
		}

		parsed_post_prev = parsed_post;
	}
	parsed_post->tail = NULL;

	//DEBUG("ref_count: %d", (*jobj)._ref_count);
	json_object_put(jobj);

	return 0;
}

/* Parse reddit comments.
 * The comments are stored in a json array[2], where [0] is the root post and
 * all replies lives under [1].
 * */
int comments_parse(char *comments_str, comment **comments_list)
{
	char *test;
	enum json_tokener_error err;
	json_tokener *tok = json_tokener_new_ex(200);
	json_object *jobj = json_tokener_parse_ex(tok, comments_str, strlen(comments_str));

	err = json_tokener_get_error(tok);

	if (err != json_tokener_success) {
		fprintf(stderr, "Error: %s\n", json_tokener_error_desc(err));
		return 1;
	}

	json_object *top_post;
	json_object *top_post_children;
	json_object *kind;
	/*json_object *top_post_children_child;
	json_object *top_post_children_child_data;*/

	//if(json_object_object_get_ex(jobj, "", &top_post) == 0)
		//return 11;
	printf("type=%d\n", json_object_get_type(jobj));
	printf("ary_len: %d\n", json_object_array_length(jobj));

	/* Get the top post from:
	 * [0] {"data"} "children"[0] {"data"} string:"selfbody" */
	if(( top_post = json_object_array_get_idx(jobj, 0)) == 0)
		return 11;

	if( json_object_object_get_ex(top_post, "data", &top_post) == 0)
		return 12;

	if( json_object_object_get_ex(top_post, "children", &top_post) == 0)
		return 13;

	if(( top_post = json_object_array_get_idx(top_post, 0)) == 0)
		return 14;

	if( json_object_object_get_ex(top_post, "data", &top_post) == 0)
		return 15;

	get_string_jobj(top_post, "selftext", &test);
	DEBUG("%s", test);
	free(test);



	/*if(( top_post_children = json_object_array_get_idx(top_post, 0)) == 0)
		return 13;*/

	printf("type=%d\n", json_object_get_type(top_post));
	//printf("%s", json_object_get_string("selfbody"));
	

	return 0;
}


