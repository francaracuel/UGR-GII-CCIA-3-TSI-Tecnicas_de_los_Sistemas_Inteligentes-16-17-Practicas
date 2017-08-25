(define (domain logistics)
	(:requirements :adl :typing :fluents )
	(:types physobj location city - object
		obj vehicle - physobj
		truck airplane - vehicle
		airport - location)
	(:predicates 	(at ?x - physobj ?l - city)
			(in ?x - obj ?t - vehicle)
			(connected ?city1 ?city2 - city))
	(:functions
	    (fuel ?t - truck)
	    (consumo-viaje ?t - truck)
	    (total-fuel ?t -truck))
			
	(:action drive-truck
	:parameters (?truck - truck ?city1 ?city2 - city)
	:precondition (and  (at ?truck ?city1)
		                (connected ?city1 ?city2)
		                (>= (- (fuel ?truck) (consumo-viaje ?truck)) 0))
	:effect (and  (at ?truck ?city2)
		   (not (at ?truck ?city1))
		   (increase (total-fuel ?truck) (consumo-viaje ?truck))
		   (decrease (fuel ?truck) (consumo-viaje ?truck))
		   (forall (?x - obj)
			(when (and (in ?x ?truck))
			      (and (not (at ?x ?city1))
				(at ?x ?city2))
		           )
			)
		    )
    )
    )