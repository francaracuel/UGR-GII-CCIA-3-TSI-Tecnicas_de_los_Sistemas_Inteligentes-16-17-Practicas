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
; Práctica 3 - Entrega 2 - Dominio del problema 4
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
    (hay-fuel-fast ?a - aircraft ?c1 - city ?c2 - city)
    (available-fuel-slow ?a - aircraft ?c1 - city ?c2 - city)
    (available-fuel-fast ?a - aircraft ?c1 - city ?c2 - city)
    ;; Ejercicio 4
    (destination ?p - person ?c - city)
    (available-duration-fast ?a - aircraft ?c1 - city ?c2 - city)
    (available-duration-slow ?a - aircraft ?c1 - city ?c2 - city)
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
    (total-fuel-used ?a - aircraft)
    (boarding-time)
    (debarking-time)
    ;; Ejercicio 3
    (fuel-limit ?a - aircraft)
    ;; Ejercicio 4
    (num-passenger ?a - aircraft)
    (capacity-passenger ?a - aircraft)
    (duration ?a - aircraft)
    (max-duration ?a - aircraft)
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
    (< (+ (total-fuel-used ?a) (* (distance ?c1 ?c2) (slow-burn ?a))) (fuel-limit ?a))
)

(:derived
    (available-fuel-fast ?a - aircraft ?c1 - city ?c2 - city)
    (< (+ (total-fuel-used ?a) (* (distance ?c1 ?c2) (fast-burn ?a))) (fuel-limit ?a))
)

;; Ejercicio 4
(:derived
    (distance ?c2 - city ?c1 - city)
    (distance ?c1 ?c2)
)

(:derived
    (available-duration-fast ?a - aircraft ?c1 - city ?c2 - city)
    (<= (+ (duration ?a) (/ (distance ?c1 ?c2) (fast-speed ?a))) (max-duration ?a))
)

(:derived
    (available-duration-slow ?a - aircraft ?c1 - city ?c2 - city)
    (<= (+ (duration ?a) (/ (distance ?c1 ?c2) (slow-speed ?a))) (max-duration ?a))
)

(:task transport-person

	:parameters (?p - person ?c - city)

    ;; Si la persona está en la ciudad no se hace nada
    (:method Case1
        :precondition (and
            (at ?p ?c)
            (destination ?p ?c)
        )

        :tasks ()
    )

    ;; Si no está en la ciudad destino, pero avión y persona están en la misma
    ;; ciudad.
    ;; En este método se embarca a todos los pasajeros, y cuando termine, vuela
    ;; a la ciudad
    (:method Case2

        :precondition (and
            (at ?p - person ?c1 - city)
            (at ?a - aircraft ?c1 - city)
            (not (destination ?p - person ?c1 - city))
        )

        :tasks (
            ;; Ejercicio 4
            (board-all ?c)
            (mover-avion ?a ?c1 ?c)
            (transport-person ?p ?c)
        )

    )

    ;; Ejercicio 4
    (:method Case3

        :precondition (and
            (destination ?p - person ?c1 - city)
        )

        :tasks (
	        (debark ?p ?a ?c1)
            (transport-person ?p ?c)
        )

    )


    ;; Ejercicio 1
    ;; Si la persona no está en la ciudad destino y el avión y persona no están
    ;; en la misma ciudad, en avión volará a la ciudad donde está la persona
    (:method Case4

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

    (:method Case5

        :precondition (and
            (in ?p - person ?a - aircraft)
            (at ?a - aircraft ?c1 - city)
            (destination ?p - person ?c2 - city)
            (different ?c1 ?c2)
        )

        :tasks (
            (mover-avion ?a ?c1 ?c2)
            (transport-person ?p ?c2)
        )

	)

    (:method Case6

        :precondition (and
            (in ?p - person ?a1 - aircraft)
            (different ?a1 - aircraft ?a2 - aircraft)
        )

        :tasks (
            (debark ?p ?a1 ?c1)
            (transport-person ?p ?c)
        )

	)

)


;; Ejercicio 4
(:task board-all

    :parameters (?c - city)

    ;; Mientras haya pasajeros en la ciudad donde está el avión, estará
    ;; embarcando.
    ;; Se llama recursivamente a esta tarea.
    (:method Case1

        :precondition (and
            (at ?p - person ?c1 - city)
            (at ?a - aircraft ?c1 - city)
            (not (destination ?p - person ?c1 - city))
        )

        :tasks (
            (board ?p ?a ?c1)
            (board-all ?c)
        )

    )

    ;; Cuando no haya más pasajeros en la ciudad donde está el avión, termina
    ;; esta tarea
    (:method Case2

        :precondition (

        )

        :tasks (

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
            (available-duration-fast ?a ?c1 ?c2)
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
            (available-duration-slow ?a ?c1 ?c2)
            (available-fuel-slow ?a ?c1 ?c2)
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


(:import "Primitivas-Zenotravel4.pddl")
)
