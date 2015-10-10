#include "annotationitem.h"

AnnotationItem::AnnotationItem()
    :id(-1), originalParse(NULL), currentParse(NULL), m_status(AnnotationItem::NotStarted)
{}

AnnotationItem::AnnotationItem(uint _id, ConllGraph *orig, ConllGraph *current)
    :id(_id), originalParse(orig), currentParse(current), m_status(AnnotationItem::NotStarted)
{}

AnnotationItem::AnnotationItem(const AnnotationItem &other)
    :id(other.id),
      originalParse(other.originalParse),
      currentParse(other.currentParse),
      m_status(other.m_status)
{}

AnnotationItem::~AnnotationItem()
{}
