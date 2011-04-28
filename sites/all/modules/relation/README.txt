
-- SUMMARY --

Describes relations between entities.

The intent of this module is to provide an API that describes the relations
between entities. For example:
  comments <--> nodes
  users <--> images

Relations can relate more than two entities, eg:
  areSiblings -> (john, jen, jack, jess)

And for 2-ary relations, relations can be directional, ie:
  bruno -> isChildOf -> boglarka

Relations are entities, so they can relate relations to other entities, for
example:
  CompanyA -> donation123 -> PartyB
  donations123 -> transaction456 -> BankC
  that is, "Company A made a donation to Political Pary B, via Bank C".

Types of relations are bundles, called predicates, in deference to the RDF
standard. The entities in the relation can be thought of as the subject and
object(s) of the relation.

  Entity relation type    = SUBJECT   + PREDICATE      + OBJECT
  Node author relation    = node      + creator        + user
  Taxonomy field relation = blog post + is tagged with + some term

Relation bundles are fieldable, so you can add any relevant fields. For
example, with the donation example above, you could add a text field denoting
"amount ($)", or a date field specifying when the donation was made.

Relations CAN NOT BE EDITED. That is, the end points that the relation relates
can not be changed. If you want to change the relation, you need to delete it
and start again. (The logic behind this is that if the relation is moving from
one entity to another, then you're actually describing a different relation).
Fields on a relation ARE editable.

An entity relation API will let us visualize the content model of a Drupal
site. With this, we could export an RDF schema of the entire content model of a
site; we could build a "content explorer" that shows the linkings from any one
piece of content (a user -> their nodes -> a particular node -> a term -> nodes
tagged with that term). The first milestone for this module is to provide simple
blocks that display specific corners of this graph, like a user's nodes, and
terms a user has used. In the future, we would like to be able to add filters to
the graph (like "a users nodes" + "only blog posts").

There is a dummy field module that, while still does not allow editing
relations, it opens the door for formatters.

--- A little more thinking/rephrasing... ---

Some relations are hard-coded properties of an entity; for example,
nodes have an author, a creation date, and a last updated date. Other
relations exist because of field values; putting a filefield on a node type
creates a relation between file entities and node type entities. An
entity_reference module could explicitly define relations between entities.

Broadly speaking, Relation plans to be able to replace all logical relation
types currently available in drupal core, and more.

For a full description of the module, visit the project page:
  http://drupal.org/project/relation
To submit bug reports and feature suggestions, or to track changes:
  http://drupal.org/project/issues/relation

--- Some related discussions/projects: ---
- http://drupal.org/node/533222 (nodereference/userreference fields in D7)
- http://drupal.pastebin.com/avnKvCD0 (notes from chx)

-- REQUIREMENTS --

See TODO.txt

-- INSTALLATION --

* Install as usual, for further information see:
  http://drupal.org/documentation/install/modules-themes/modules-7
* By default, the module is an API module only. If you want a UI, enable the
  relation_dropzone module.

-- CONFIGURATION --

* Go to admin/structure/relation, and create a new relation type. Add fields if
  neccesary.
* Enable the relation_dropzone block if it is not enable on install -- it tries
to insert itself after the system management block if that one is enabled.

-- USAGE --

* To use the relation_dropzone block, go to any page that loads entities, and
  the entity selector will appear.
* "Pick" as many entities as you need for your relation type (between min_ and
  max_arity in the appropriate relation bundle)
* Click "Create Relation", your relation will be created, and you will be given
  a link to the relation page.
* Here you can view the relation, and edit it to add or change field data.

-- CONTACT --

Current maintainers:
* Daniel F. Kudwien (sun) - http://drupal.org/user/54136
* Ned Haughton (naught101) - http://drupal.org/user/44216
* Karoly Negesi (chx) - http://drupal.org/user/9446
