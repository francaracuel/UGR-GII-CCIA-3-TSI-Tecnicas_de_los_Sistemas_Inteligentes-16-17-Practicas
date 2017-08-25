(define (domain monkey-domain)	       ; Comment: adding location caused fail
  (:requirements :strips :equality :typing)
  (:types  monkey banana box - locatable
          location)
 
  (:predicates 
	       (on-floor ?x - monkey )
	       (at ?x - locatable ?y - location)
	       (onbox ?x - monkey ?y - location)
	       (hasbananas ?x - monkey)
	       
	       )
  (:action GRAB-BANANAS
	     :parameters (?m - monkey ?y - location)
	     :precondition (and (onbox ?m  ?y))
	     :effect (hasbananas ?m))
  (:action CLIMB_BOX
	     :parameters (?m - monkey ?bo - box ?l - location ?ba - banana)
	     :precondition (and (at ?m  ?l) (at ?bo ?l) (at ?ba ?l))
	     :effect (and (not(on-floor ?m)) (onbox ?m ?l)))
  (:action PUSH_MOVE
	     :parameters (?m - monkey ?bo - box ?l - location ?ba - banana)
	     :precondition (and (at ?m  ?l) (at ?bo ?l) (at ?ba ?l))
	     :effect (onbox ?m ?l))
)
