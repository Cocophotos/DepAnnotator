#DepAnnotator

*WARNING*: This README is a work in progress and will not be detailed for now.

# What is it?

DepAnnotator is an annotator tool designed to annotate dependency graphs (e.g. predicate-argument structures). It has been designed with Qt and should work with Qt 4.4 and above (up to the last version of Qt 5.x). It has been extensively tested on Ubuntu 12.04 until 15.04, MacOS X 10.6 until 10.10 and Windows Vista until Windows Seven.

# How to compile it?

Go to the directory with the \*.pro file and execute the following commands:

```bash
qmake
make
```

You can also use Qt Creator to import the project and compile the software.

# How to use it?

TBA

# Features

This is not a complete list of features:

- Backward compatible with [DGAnnotator](http://medialab.di.unipi.it/Project/QA/Parser/DgAnnotator/) XML annotation scheme file. (See contrib directory for some annotation schemes)
- You can annotate: 
    * tokens, lemma, POS (coarse and fine-grained), morphological features by clicking on a token,
    * dependency lables by clicking on an edge,
    * add / remove / invert edges.
- All dependency labels, POS, possible morphological features are defined in the XML annotation scheme file.
- Each modification is kept with a revision number (View > View Revisions) to see the panel and go back to a previous revision.
- Panels can be rearranged or detached from main interface (to be used with multiple screens).

Some screenshots:

- Main interface:
![Main interface](https://cloud.githubusercontent.com/assets/1861291/10717105/b6e88160-7b4e-11e5-890a-5114ecd05782.png)
- Revision panel: 
![Revision panel](https://cloud.githubusercontent.com/assets/1861291/10717108/b6eb783e-7b4e-11e5-908e-a86be4a85f17.png)
- Labels edition:
![Labels edition](https://cloud.githubusercontent.com/assets/1861291/10717106/b6ea0198-7b4e-11e5-904a-82fb329e133f.png)
- Token (lemma, POS, morphological features) edition:
![Token edition](https://cloud.githubusercontent.com/assets/1861291/10717107/b6ea8988-7b4e-11e5-9d1e-32f2f671d1ab.png)

# Annotation files

- Thanks to [Djamé Seddah](http://pauillac.inria.fr/~seddah/) for the french DGA file.
- Thanks to [Teresa Lynn](http://www.nclt.dcu.ie/~tlynn/) for the Universal Dependencies DGA file.

# How to cite

If you're using the software, please cite the following work :

Corentin Ribeyre, Méthodes d’Analyse Supervisée pour l’Interface Syntaxe-Sémantique, PhD Thesis, Université Paris 7 Diderot, 2015 (to appear).

```bib
@phdthesis{ribeyre:2015:phd,
title = {{Méthodes d'Analyse Supervisée pour l'Interface Syntaxe-Sémantique}},
author = {Ribeyre, Corentin},
school = {Universit{\'{e}} Paris 7 Diderot},
year = {2015},
note = {to appear}
}
```
