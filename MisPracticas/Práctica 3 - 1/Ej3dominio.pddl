;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; Francisco Javier Caracuel Beltrán
;;
;; Técnicas de los Sistemas Inteligentes
;;
;; Curso 2016/2017
;;
;; Grupo 3
;;
;; Ejercicio 3 - Dominio
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(define (domain ej3-domain)

    (:requirements :strips :equality :typing :fluents)

    ; Se establecen los objetos
    (:types element zone orientation - object
        person obj - element
        player character - person
        tool - obj

        ; Cada zona es de un tipo
        ;forest water precipice sand stone - type

        ; Cada personaje es un subtipo de character
        ;princess prince witch professor leo - character

        ; Cada objeto es un subtipo de obj
        ;oscar apple rose algorithm gold - obj

        ; Para la zona geográfica se crea un objeto indicando la orientación
        ;north south east west - orientation

    )

    ; Se establecen los predicados para que se pueda describir en el problema
    ; el estado del mundo
    (:predicates

        ; No se indica que una zona está relacionada con otra. Implicitamente
        ; se entiende que si una zona está al norte de otra, ambas están
        ; relacionadas
        ; Indica que la zona 1 está al norte/sur/este/oeste de la zona 2
        (orientated ?z1 - zone ?o - orientation ?z2 - zone)

        ; Indica el lugar al que está mirando el jugador
        (lookAt ?p - player ?o - orientation)

        ; Indica en que zona se encuentra un objeto/jugador/personaje
        (at ?e - element ?z - zone)

        ; Indica que el jugador/personaje tiene un objeto
        (has ?p - person ?o - obj)

        ; Se supone que el jugador no puede llevar más de un objeto a la vez,
        ; por lo que se indica si tiene la mano vacía o no
        (EH)

        ; El jugador tiene una mochila y se indicará si tiene un objeto en ella
        (backpack ?o - obj)

        ; Las zonas ahora son de un cierto tipo. Se indica qué tipo son con
        ; este predicado
        (is ?z - zone ?t - type)

    )

    ; Se declaran las funciones
    (:functions

        ; Indica la distancia que hay entre la zona 1 y la zona 2
        (distance ?z1 - zone ?z2 - zone)

        ; Se guarda una variable que indica la distancia total que va
        ; recorriendo y será la que se minimice
        (totalDistance)

    )

    ; Se establecen las acciones que puede hacer el jugador en el entorno
    ; Girar a la derecha
    (:action TURN_RIGHT

        ; Parámetros que se usan en la acción
        :parameters (?p - player)

        ; No existen precondiciones

        ; Efectos  que produce la acción
        :effect (and

            (when (lookAt ?p north)
                (and
                    (lookAt ?p east)
                    (not (lookAt ?p north))))

            (when (lookAt ?p east)
                (and
                    (lookAt ?p south)
                    (not (lookAt ?p east))))

            (when (lookAt ?p south)
                (and
                    (lookAt ?p west)
                    (not (lookAt ?p south))))

            (when (lookAt ?p west)
                (and
                    (lookAt ?p north)
                    (not (lookAt ?p west))))

        )

    )

    ; Girar a la izquierda
    (:action TURN_LEFT

        ; Parámetros que se usan en la acción
        :parameters (?p - player)

        ; No existen precondiciones

        ; Efectos  que produce la acción
        :effect (and

            (when (lookAt ?p north)
                (and
                    (lookAt ?p west)
                    (not (lookAt ?p north))))

            (when (lookAt ?p west)
                (and
                    (lookAt ?p south)
                    (not (lookAt ?p west))))

            (when (lookAt ?p south)
                (and
                    (lookAt ?p east)
                    (not (lookAt ?p south))))

            (when (lookAt ?p east)
                (and
                    (lookAt ?p north)
                    (not (lookAt ?p east))))

        )
    )

    ; Ir de una zona a otra
    (:action GO

        ; Parámetros que se usan en la acción
        :parameters (?p - player ?z1 - zone ?z2 - zone ?o - orientation)

        ; Precondiciones necesarias
        ; El jugador debe estar situado en la zona 1
        ; La orientación del jugador debe ser la misma que la orientación de la
        ; zona 2 sobre la zona 1
        ; Si la zona no es bosque o agua.
        ; Si la zona es bosque tiene que tener unas zapatillas
        ; Si la zona es agua tiene que tener un bikini
        ; Si la zona es un precipio no puede ir
        :precondition (and
            (at ?p ?z1)
            (lookAt ?p ?o)
            (orientated ?z2 ?o ?z1)
            (or
                (and
                    (not (is ?z2 forest))
                    (not (is ?z2 water))
                )
                (and
                    (is ?z2 forest)
                    (or
                        (has ?p shoe)
                    )
                )
                (and
                    (is ?z2 water)
                    (or
                        (has ?p bikini)
                    )
                )
            )
            (not (is ?z2 precipice))
        )

        ; Efectos que produce la acción
        ; Está en la nueva zona y no está en la antigua zona
        :effect (and
            (at ?p ?z2)
            (not (at ?p ?z1))
            (increase (totalDistance) (distance ?z1 ?z2))
        )

    )

    ; Coger un objeto
    (:action GET

        ; Parámetros que se usan en la acción
        :parameters (?p - player ?o - obj ?z - zone)

        ; Precondiciones necesarias:
        ; El jugador y el objeto deben estar situados en la misma zona y el
        ; jugador debe tener la mano vacía
        ; El jugador no puede tener ese mismo objeto. El jugador no podría tener
        ; dos veces el mismo objeto. Se pone esta restricción porque si no,
        ; habría que llevar un contador de cuantos objetos de distinto tipo
        ; tiene encima
        :precondition (and
            (at ?p ?z)
            (at ?o ?z)
            (EH)
            (not (has ?p ?o))
        )

        ; Efectos que produce la acción:
        ; El jugador coge el objeto y el objeto ya no está en la zona. El
        ; jugador no tiene la mano vacía
        :effect (and
            (has ?p ?o)
            (not (at ?o ?z))
            (not (EH))
        )

    )

    ; Dejar un objeto
    (:action RELEASE

        ; Parámetros que se usan en la acción
        :parameters (?p - player ?o - obj ?z - zone)

        ; Precondiciones necesarias:
        ; El jugador debe tener el objeto y no puede tener la mano vacía y debe
        ; estar en una zona. El objeto no puede estar en la mochila
        :precondition (and
            (has ?p ?o)
            (not (EH))
            (at ?p ?z)
            (not (backpack ?o))
        )

        ; Efectos que produce la acción:
        ; El jugador no tiene el objeto y el objeto está en esa zona. El jugador
        ; tiene la mano vacía
        :effect (and
            (not (has ?p ?o))
            (at ?o ?z)
            (EH)
        )

    )

    ; Meter un objeto en la mochila
    (:action PUT_BACKPACK

        ; Parámetros que se usan en la acción
        :parameters (?p - player ?o - obj)

        ; Precondiciones necesarias:
        ; El jugador debe tener el objeto en la mano y la mano no puede estar
        ; vacía
        :precondition (and
            (not (EH))
            (has ?p ?o)
            (not
                (exists (?o1 - obj)
                    (and
                        (backpack ?o1)
                    )
                )
            )
        )

        ; Efectos que produce la acción:
        ; El jugador tiene la mano vacía y el objeto está en la mochila
        :effect (and
            (EH)
            (backpack ?o)
        )

    )

    ; Sacar un objeto de la mochila
    (:action POP_BACKPACK

        ; Parámetros que se usan en la acción
        :parameters (?p - player ?o - obj)

        ; Precondiciones necesarias:
        ; El jugador debe tener la mano vacía y tener el objeto en la mochila
        :precondition (and
            (EH)
            (has ?p ?o)
            (backpack ?o)
        )

        ; Efectos que produce la acción:
        ; El jugador no tiene la mano vacía y el objeto no está en la mochila
        :effect (and
            (not (EH))
            (not (backpack ?o))
        )

    )

    ; Entregar un objeto a un personaje
    (:action GIVE

        ; Parámetros que se usan en la acción
        :parameters (?p - player ?c - character ?o - obj ?z - zone)

        ; Precondiciones necesarias:
        ; El jugador debe tener el objeto y no tener la mano vacía.
        ; El jugador y el personaje deben estar en la misma zona
        :precondition (and
            (has ?p ?o)
            (not (EH))
            (at ?p ?z)
            (at ?c ?z)
            ;(and
                ;(not (= ?o shoe))
                ;(not (= ?o bikini))
            ;)
        )

        ; Efectos que produce la acción:
        ; El jugador ya no tiene el objeto y tiene la mano vacía.
        ; El personaje tiene el objeto
        :effect (and
            (not (has ?p ?o))
            (EH)
            (has ?c ?o)
        )

    )

)

; Fin del dominio
