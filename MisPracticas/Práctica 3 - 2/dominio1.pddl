;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; Francisco Javier Caracuel Beltrán
;
; Técnicas de los Sistemas Inteligentes
;
; Curso 2016/2017
;
; Grupo 3
;
; Práctica 3 - Entrega 2 - Dominio de los problemas 1, 2 y 3
;
; Grado en Ingeniería Informática
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(define (domain zeno-travel)

(:requirements
    :typing
    :fluents
    :derived-predicates
    :negative-preconditions
    :universal-preconditions
    :disjuntive-preconditions
    :conditional-effects
    :htn-expansion
    ; Requisitos adicionales para el manejo del tiempo
    :durative-actions
    :metatags
)

(:types aircraft person city - object)

(:constants slow fast - object)

; either indica que puede haber varios tipos a la vez
(:predicates
    (at ?x - (either person aircraft) ?c - city)
    (in ?p - person ?a - aircraft)
    (different ?x ?y)
    (igual ?x ?y)
    (hay-fuel-slow ?a ?c1 ?c2)
    ;; Ejercicio 3
    (hay-fuel-fast ?a ?c1 ?c2)
    (available-fuel-slow ?a ?c1 ?c2)
    (available-fuel-fast ?a ?c1 ?c2)
)

(:functions
    (fuel ?a - aircraft)
    (distance ?c1 - city ?c2 - city)
    (slow-speed ?a - aircraft)
    (fast-speed ?a - aircraft)
    (slow-burn ?a - aircraft)
    (fast-burn ?a - aircraft)
    (capacity ?a - aircraft)
    (refuel-rate ?a - aircraft)
    (total-fuel-used)
    (boarding-time)
    (debarking-time)
    ;; Ejercicio 3
    (fuel-limit)
)

;; El consecuente "vacío" se representa como "()" y significa "siempre verdad"
(:derived
    (igual ?x ?x) ()
)

(:derived
    (different ?x ?y) (not (igual ?x ?y))
)

;; Este literal derivado se utiliza para deducir, a partir de la información en
;; el estado actual, si hay fuel suficiente para que el avión ?a vuele de la
;; ciudad ?c1 a la ?c2 el antecedente de este literal derivado comprueba si el
;; fuel actual de ?a es mayor que 1. En este caso es una forma de describir que
;; no hay restricciones de fuel. Pueden introducirse una restricción más copleja
;; si en lugar de 1 se representa una expresión más elaborada (esto es objeto de
;; los siguientes ejercicios).
(:derived
    (hay-fuel-slow ?a - aircraft ?c1 - city ?c2 - city)
    ;; Ejercicio 2
    (>= (fuel ?a) (* (distance ?c1 ?c2) (slow-burn ?a)))
)

;; Ejercicio 3
;; Se crea otro literal derivado para comprobar si hay fuel para viajar a alta
;; velocidad
(:derived
    (hay-fuel-fast ?a - aircraft ?c1 - city ?c2 - city)
    (>= (fuel ?a) (* (distance ?c1 ?c2) (fast-burn ?a)))
)

;; Se crea un literal derivado para no sobrepasar el límite de fuel
(:derived
    (available-fuel-slow ?a - aircraft ?c1 - city ?c2 - city)
    (< (+ (total-fuel-used) (* (distance ?c1 ?c2) (slow-burn ?a))) (fuel-limit))
)

(:derived
    (available-fuel-fast ?a - aircraft ?c1 - city ?c2 - city)
    (< (+ (total-fuel-used) (* (distance ?c1 ?c2) (fast-burn ?a))) (fuel-limit))
)

(:task transport-person

	:parameters (?p - person ?c - city)

    ;; Si la persona está en la ciudad no se hace nada
    (:method Case1
        :precondition (at ?p ?c)
        :tasks ()
    )

    ;; Si no está en la ciudad destino, pero avión y persona están en la misma
    ;; ciudad
    (:method Case2

        :precondition (and
            (at ?p - person ?c1 - city)
            (at ?a - aircraft ?c1 - city)
        )

        :tasks (
            (board ?p ?a ?c1)
            (mover-avion ?a ?c1 ?c)
	        (debark ?p ?a ?c)
        )

    )

    ;; Ejercicio 1
    ;; Si la persona no está en la ciudad destino y el avión y persona no están
    ;; en la misma ciudad, en avión volará a la ciudad donde está la persona
    (:method Case3

        :precondition (and
            (at ?p - person ?c1 - city)
            (at ?a - aircraft ?c2 - city)
            (different ?c1 ?c2)
        )

        :tasks (
            (mover-avion ?a ?c2 ?c1)
            (transport-person ?p ?c)
        )

	)

)

(:task mover-avion

    :parameters (?a - aircraft ?c1 - city ?c2 -city)

    ;; Ejercicio 3
    (:method fuel-suficiente-fast3

        :precondition (and
            (hay-fuel-fast ?a ?c1 ?c2)
            (different ?c1 ?c2)
            (available-fuel-fast ?a ?c1 ?c2)
        )

        :tasks(
            (zoom ?a ?c1 ?c2)
        )

    )

    ;; Ejercicio 3
    (:method fuel-suficiente-slow3

        :precondition (and
            (hay-fuel-slow ?a ?c1 ?c2)
            (different ?c1 ?c2)
            (available-fuel-slow ?a ?c1 ?c2)
        )

        :tasks(
            (fly ?a ?c1 ?c2)
        )

    )

    ;; Ejercicio 2
    (:method fuel-suficiente-slow2

        :precondition (and
            (hay-fuel-slow ?a ?c1 ?c2)
            (different ?c1 ?c2)
        )

        :tasks(
            (fly ?a ?c1 ?c2)
        )

    )

    ;; Ejercicio 3
    ;; Si no hay fuel suficiente para hacer un viaje a alta velocidad, se
    ;; reposta
    (:method no-fuel-suficiente-fast

        :precondition (and
            (not (hay-fuel-fast ?a ?c1 ?c2))
            (different ?c1 ?c2)
        )

        :tasks(
            (refuel ?a ?c1)
            (mover-avion ?a ?c1 ?c2)
        )

    )

    ;; Ejercicio 2
    ;; Si no hay fuel suficiente para hacer un viaje, se reposta
    (:method no-fuel-suficiente-slow

        :precondition (and
            (not (hay-fuel-slow ?a ?c1 ?c2))
            (different ?c1 ?c2)
        )

        :tasks(
            (refuel ?a ?c1)
            (mover-avion ?a ?c1 ?c2)
        )

    )

)

(:import "Primitivas-Zenotravel.pddl")
)
