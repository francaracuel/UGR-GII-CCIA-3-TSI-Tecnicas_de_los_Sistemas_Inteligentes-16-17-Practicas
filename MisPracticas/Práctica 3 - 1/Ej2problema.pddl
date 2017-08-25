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
;; Ejercicio 2 - Problema
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(define (problem ej2-problem)

    ; Se indica el dominio del problema
    (:domain ej2-domain)

    ; Objetos que forman parte del problema
    (:objects

        ; Jugador (robot)
        p - player

        ; Personajes
        princess prince witch professor leo - character

        ; Objetos
        oscar apple rose algorithm gold - obj

        ; Orientaciones
        north south east west - orientation

        ; Zonas
        z1 z2 z3 z4 z5 z6 z7 z8 z9 z10 z11 z12 z13 z14 z15 z16 z17 z18 - zone
        z19 z20 z21 z22 z23 z24 z25 - zone

    )

    ; Estado inicial del problema
    (:init

        ; Indica el estado del jugador
        (at p z1)
        (lookAt p west)
        (EH)

        ; Indica el estado de los personajes
        (at princess z5)
        (at prince z13)
        (at witch z20)
        (at professor z7)
        (at leo z16)

        ; Indica el estado de los objetos
        (at oscar z19)
        (at apple z22)
        (at rose z6)
        (at algorithm z9)
        (at gold z21)
        ; Se pone más de un objeto igual porque tarda mucho en el cálculo si no
        ; es así al tener 5x5 zonas
        (at oscar z2)
        (at apple z4)
        (at rose z14)
        (at algorithm z11)
        (at gold z25)

        ; Indica el estado del mapa
        (orientated z1 west z2)
        (orientated z1 north z6)
        (orientated z2 east z1)
        (orientated z2 north z7)
        (orientated z3 north z8)
        (orientated z3 west z4)
        (orientated z4 north z9)
        (orientated z4 west z5)
        (orientated z4 east z3)
        (orientated z5 east z4)
        (orientated z5 north z10)
        (orientated z6 north z11)
        (orientated z6 south z1)
        (orientated z7 north z12)
        (orientated z7 west z8)
        (orientated z7 south z2)
        (orientated z8 east z7)
        (orientated z8 south z3)
        (orientated z8 west z9)
        (orientated z9 north z14)
        (orientated z9 south z4)
        (orientated z9 east z8)
        (orientated z10 north z15)
        (orientated z10 south z5)
        (orientated z11 north z16)
        (orientated z11 west z12)
        (orientated z11 south z6)
        (orientated z12 west z13)
        (orientated z12 south z7)
        (orientated z12 north z17)
        (orientated z12 east z11)
        (orientated z13 north z18)
        (orientated z13 east z12)
        (orientated z13 west z14)
        (orientated z14 north z19)
        (orientated z14 west z15)
        (orientated z14 south z9)
        (orientated z14 east z13)
        (orientated z15 north z20)
        (orientated z15 south z10)
        (orientated z15 east z14)
        (orientated z16 north z21)
        (orientated z16 south z11)
        (orientated z17 north z22)
        (orientated z17 south z12)
        (orientated z17 west z18)
        (orientated z18 west z19)
        (orientated z18 south z13)
        (orientated z18 east z17)
        (orientated z19 north z24)
        (orientated z19 south z14)
        (orientated z19 east z18)
        (orientated z19 west z20)
        (orientated z20 south z15)
        (orientated z20 east z19)
        (orientated z20 north z25)
        (orientated z21 south z16)
        (orientated z21 west z22)
        (orientated z22 south z17)
        (orientated z22 west z23)
        (orientated z22 east z21)
        (orientated z23 west z24)
        (orientated z23 east z22)
        (orientated z24 south z19)
        (orientated z24 west z25)
        (orientated z24 east z23)
        (orientated z25 east z24)
        (orientated z25 south z20)

        ; Se indica la distancia total (será 0)
        (= (totalDistance) 0)

        ; Se indica la distancia que hay entre las zonas
        (= (distance z1 z2) 2)
        (= (distance z1 z6) 1)
        (= (distance z2 z1) 5)
        (= (distance z2 z7) 2)
        (= (distance z3 z8) 1)
        (= (distance z3 z4) 1)
        (= (distance z4 z9) 2)
        (= (distance z4 z5) 3)
        (= (distance z4 z3) 2)
        (= (distance z5 z4) 2)
        (= (distance z5 z10) 2)
        (= (distance z6 z11) 2)
        (= (distance z6 z1) 4)
        (= (distance z7 z12) 4)
        (= (distance z7 z8) 4)
        (= (distance z7 z2) 5)
        (= (distance z8 z7) 2)
        (= (distance z8 z3) 1)
        (= (distance z8 z9) 1)
        (= (distance z9 z14) 1)
        (= (distance z9 z4) 2)
        (= (distance z9 z8) 4)
        (= (distance z10 z15) 2)
        (= (distance z10 z5) 2)
        (= (distance z11 z16) 1)
        (= (distance z11 z12) 4)
        (= (distance z11 z6) 5)
        (= (distance z12 z13) 4)
        (= (distance z12 z7) 6)
        (= (distance z12 z17) 2)
        (= (distance z12 z11) 2)
        (= (distance z13 z18) 2)
        (= (distance z13 z12) 1)
        (= (distance z13 z14) 3)
        (= (distance z14 z19) 4)
        (= (distance z14 z15) 2)
        (= (distance z14 z9) 3)
        (= (distance z14 z13) 3)
        (= (distance z15 z20) 4)
        (= (distance z15 z10) 1)
        (= (distance z15 z14) 1)
        (= (distance z16 z21) 2)
        (= (distance z16 z11) 4)
        (= (distance z17 z22) 2)
        (= (distance z17 z12) 1)
        (= (distance z17 z18) 3)
        (= (distance z18 z19) 2)
        (= (distance z18 z13) 1)
        (= (distance z18 z17) 2)
        (= (distance z19 z24) 1)
        (= (distance z19 z14) 5)
        (= (distance z19 z18) 4)
        (= (distance z19 z20) 2)
        (= (distance z20 z15) 4)
        (= (distance z20 z19) 1)
        (= (distance z20 z25) 1)
        (= (distance z21 z16) 5)
        (= (distance z21 z22) 1)
        (= (distance z22 z17) 3)
        (= (distance z22 z23) 1)
        (= (distance z22 z21) 2)
        (= (distance z23 z24) 3)
        (= (distance z23 z22) 1)
        (= (distance z24 z19) 3)
        (= (distance z24 z25) 6)
        (= (distance z24 z23) 3)
        (= (distance z25 z24) 5)
        (= (distance z25 z20) 1)

    )

    ; Objetivo final del problema
    (:goal

        ; El objetivo es que cada personaje tenga un objeto sin importar el
        ; que sea
        (exists (?o1 ?o2 ?o3 ?o4 ?o5 - obj)
            (and
                (has princess ?o1)
                (has prince ?o2)
                (has witch ?o3)
                (has professor ?o4)
                (has leo ?o5)
            )
        )

        ; Expresamente se indica que cada personaje debe tener su objeto
        ;(and
            ;(has princess rose)
            ;(has prince gold)
            ;(has witch apple)
            ;(has professor algorithm)
            ;(has leo oscar)
            ;(at p z20)
        ;)

    )

    ; Optimización que debe hacer
    ; En este caso se optimiza que la distancia total sea mínima
    (:metric minimize (totalDistance))

)

; Fin del problema
