;; Extensión para la mapa
extensions [ gis ]

turtles-own
  [ contagiado?                ;; Es verdadero si la tortuga contrae el virus. Corresponde al momento de contagio
    enfermo?                   ;; Se hace verdadero después del periodo de incubación
    tiempo-contagiado          ;; Cuanto tiempo lleva el agente infectado
    recuperado?                ;; Verdadero si la tortuga se recupera
    muerto?                    ;; Si se muere o no el agente
    duración-contagio          ;; Tiempo que conserva el virus dentro del agente
    edad                       ;; Años de la persona
    familiares                 ;; Lista de familiares
    casax                      ;; Centro de un circulo representando la casa eje x
    casay                      ;; Centro de un circulo representando la casa eje y
]

patches-own[
  infectado?         ;; Si el patch es infeccioso
  duración-infección ;; Cuanto tiempo dura la infección en el patch
  tiempo-infectado   ;; Cuanto tiempo lleva el patch infectado.
]

globals
  [ %infectados          ;; Porcentaje de población infectado
    %inmune              ;; Porcentaje de población inmune
    muertos_previo       ;; Número de muertos ayer
    muertos_actual       ;; Número de muertos hoy
    muertos              ;; Número de muertos total
    dia_previo           ;; Número de contagiados ayer
    dia_actual           ;; Número de contagiados hoy
    imagen-tortuga       ;; Figura para la tortuga
    probs_muerte         ;; Probabilidades de muerte
    edades               ;; Rango de edades
    contar_dia           ;; Contar 24 ticks
    probs_familia        ;; Proporción del número de personas por familia
    numero_familia       ;; Numero de personas por familia
    primera_corrida      ;; Verificar si es la primera vez
    contorno             ;; Mapa del contorno de Medellín
]

;; El setup de cada una de las variables
to setup
  clear-all
  ;; Renderización del mapa
  setup-gis

  ;; Inicializar variables
  set muertos 0
  set dia_previo 0
  set muertos_previo 0
  set contar_dia 0
  set probs_muerte (list 0 0.2 0.2 0.2 0.4 1.3 3.6 8 14.8)
  set edades (list (list 0 9) (list 10 19) (list 20 29) (list 30 39) (list 40 49) (list 50 59) (list 60 69) (list 70 79) (list 80 121))
  set probs_familia (list 19 23 24 19 8 7)
  set numero_familia (list 1 2 3 4 5 6)
  set primera_corrida false

  ;; Creación de la simulación
  setup-parches
  setup-tortugas
  actualizar-variables-global
  actualizar-display
  reset-ticks
end

to setup-gis
  set contorno gis:load-dataset "maps/medellin_merged.shp"
  gis:set-world-envelope-ds gis:envelope-of contorno
  gis:set-drawing-color black
  gis:draw contorno 1.0
end
;; Creación inicial de tortugas
to setup-tortugas
  create-turtles tamaño-de-población [
    set tiempo-contagiado 0
    set recuperado? false
    set shape "dot"
    set imagen-tortuga "circle"   ;; Si son muchos más debe cambiarse por "dot"
    set muerto? false
    set contagiado? false
    set tiempo-contagiado 0
    set familiares (list self)
    obtener-edad
  ]
  crear-familias
  ir-a-la-casa
  ask n-of init-infectados turtles [ contagiarse ]
end

;; Obtener edad para la tortuga
to obtener-edad
  ;; Calcular edad de tortuga
  let probs (list 14.43 16.9 17.28 14.87 12.21 11.04 7.28 3.93 2.06 )
  let indice 0
  let acum 0
  let edad0 0
  let eleccion random-float 100
  while [indice < length probs] [
    let elem (item indice probs)
    set acum (acum + elem)
    if eleccion < acum [
      let elem0 (item indice edades)
      set edad0 ((item 0 elem0) + random ((item 1 elem0) - (item 0 elem0)) + 1)
      set indice (length probs)
    ]
    set indice (indice + 1)
  ]
  set edad edad0

  ;; Calcular tamaño
  ifelse edad < 20 [
    set size 2
  ]
  [ifelse edad < 60 [
    set size 3
  ]
  [set size 4]]
end

;; Crear familias de tortuga
to crear-familias
  ;; Asignar familia a los niños
  ask turtles with [ edad < 18 ] [
    if length familiares <= 1 [
      ;; Probabilidad condicional
      let probs_niños []
      let total (sum (sublist probs_familia 1 (length probs_familia)))
      let indice 1
      while [ indice < length probs_familia - 1 ] [
        set probs_niños (insert-item (length probs_niños) probs_niños ((item indice probs_familia) * 100 / total))
        set (indice) (indice + 1)
      ]
      set probs_niños (insert-item (length probs_niños) probs_niños (100 - (sum probs_niños)))

      ;; Ver cuantas personas hay en la familia de la tortuga
      set indice 0
      let acum 0
      let num 0
      let eleccion random-float 100
      while [indice < length probs_niños] [
        let elem (item indice probs_niños)
        set acum (acum + elem)
        if eleccion < acum [
          set num ((item (indice + 1) numero_familia) - 2)
          set indice (length probs_familia)
        ]
        set indice (indice + 1)
      ]

      ;; Seleccionar familiares
      let adulto (one-of other turtles with [ edad >= 18 and length familiares <= 1 ])
      set familiares (insert-item (length familiares) familiares adulto)
      if num > 0 [
        let opciones other turtles with [ length familiares <= 1 ]
        let resto-familia n-of (min (list num (count opciones))) opciones
        let familiares-aux familiares
        ask resto-familia [
          set familiares-aux (insert-item (length familiares-aux) familiares-aux self)
        ]
        set familiares familiares-aux

        ;; Seleccionar familiares para los que ya se hicieron
        foreach familiares [
          set familiares familiares-aux
        ]
      ]
    ]
  ]

  ask turtles with [ length familiares <= 1 ] [
    ;; Ver cuantas personas hay en la familia de la tortuga
      let indice 1
      let acum 0
      let num 0
      let eleccion random-float 100
      while [indice < length probs_familia] [
        let elem (item indice probs_familia)
        set acum (acum + elem)
        if eleccion < acum [
          set num ((item indice numero_familia) - 1)
          set indice (length probs_familia)
        ]
        set indice (indice + 1)
      ]

      ;; Asignar familias
      if num > 0 [
        let opciones other turtles with [ length familiares <= 1 ]
        let resto-familia n-of (min (list num (count opciones))) opciones
        set indice 0
        let familiares-aux familiares
        ask resto-familia [
          set familiares-aux (insert-item (length familiares-aux) familiares-aux self)
        ]
        set familiares familiares-aux

        ;; Seleccionar familiares para los que ya se hicieron
        let familiares_aux familiares
        foreach familiares [
          set familiares familiares_aux
        ]
      ]
  ]
end

;; Crear casa y mandar las personas a ella
to ir-a-la-casa
  let vistos []
  let centros []
  let colores 0
  ask turtles [
    if not member? self vistos [
      ;; Centro distintos
      let centrox random-xcor
      let centroy random-ycor
      let indice 0
      while [ indice < length centros ] [
        ;; Que las centros no estén incluidos
        ;;while [ sqrt((centrox - (item indice centros)) ^ 2 + (centroy - (item (indice + 1) centros)) ^ 2) < 1 ] [
         ;; set centrox random-xcor
         ;; set centroy random-ycor
        ;;]
        set indice (indice + 2)
      ]

      while [ not gis:contained-by? (patch centrox centroy) contorno ] [
          set centrox random-xcor
          set centroy random-ycor
      ]

      ;; Posicionar cada familiar en la casa
      ;;set centros (insert-item (length centros) centros centrox)
      ;;set centros (insert-item (length centros) centros centroy)
      foreach familiares [
        set vistos (insert-item (length vistos) vistos self)
        set casax centrox
        set casay centroy
        let posicionx (centrox + (-0.5 + 1 * random-float 1))
        let posiciony (centroy + (-0.5 + 1 * random-float 1))
        setxy posicionx posiciony
        while [ not gis:contained-by? self contorno ] [
          set posicionx (centrox + (-0.5 + 1 * random-float 1))
          set posiciony (centroy + (-0.5 + 1 * random-float 1))
          setxy posicionx posiciony
        ]
        ask patch-here [
          set pcolor 128
        ]
      ]
    ]
  ]
end

;; Creación de patches
to setup-parches
  ask patches [
    set pcolor white
    set tiempo-infectado 0
    set infectado? false
  ]
end

;; Inicializar y actualizar variables globales
to actualizar-variables-global
  if count turtles > 0 [
    set %infectados (count turtles with [ contagiado? ] / count turtles) * 100
    set %inmune (count turtles with [ recuperado? ] / count turtles) * 100
  ]
  if contar_dia = 24 [
    set dia_actual (count turtles with [contagiado?])
    set muertos_actual muertos
    set contar_dia -1
  ]
end

;; Correr la simulación
to correr
  if not primera_corrida [
    ask patches [
      set pcolor white
    ]
    set primera_corrida true
  ]
  if contar_dia = 24 [
    set dia_previo (count turtles with [contagiado?])
    set muertos_previo muertos
  ]

  ask turtles [
    mover
    if contagiado? [ recuperar-o-morir
                     infectar
    ]
  ]
  actualizar-patch
  infectar-persona-patch
  actualizar-variables-global
  actualizar-display
  set contar_dia (contar_dia + 1)
  tick
end

;; Observar muerte o recuperación de tortuga
to recuperar-o-morir
  ifelse tiempo-contagiado > duración-contagio
    [ifelse muerto?
      [
        set muertos (muertos + 1)
        die
      ]
      [
        set contagiado? false
        set recuperado? true
        set tiempo-contagiado 0
      ]
  ]
  [
    set tiempo-contagiado (tiempo-contagiado + 1)
  ]
end

;; Infectar personas cerca y patches
to infectar
  ask other turtles in-radius radio-infección with [ not contagiado? and not recuperado? ]
    [ if random-float 100 < tasa-infección
      [ contagiarse ] ] ;; Contagiar agente cerca

  ask patch-here [
    set infectado? true
    set duración-infección (random-exponential duración-virus-patch) ;; Tiempo infección patch
  ]
end

;; Contagiar a la persona con el virus y decidir su recuperación
to contagiarse
  set contagiado? true

  ;; Seleccionar probabilidad de muerte
  let indice 0
  let indice_edad 0
  while [indice < length probs_muerte] [
    let elem (item indice edades)
    if (item 0 elem) <= edad and edad <= (item 1 elem) [
      set indice_edad indice
      set indice (length probs_muerte)
    ]
    set indice (indice + 1)
  ]

  ;; Observar si se muere
  if random-float 100 < (item indice_edad probs_muerte) [
    set muerto? true
  ]

  ;; Calcular tiempo de contagio
  ifelse muerto? [
    set duración-contagio (14 * 24) ;; El tiempo que se demora en morir
  ]           ;; TODO: Distribución de tiempo de muerte o infección.
  [
    set duración-contagio (14 * 24) ;; El tiempo que se demora en recuperarse
  ]
end

;; Cambiar el estado del patch a infeccioso o no
to actualizar-patch
  ask patches [
    if infectado? [
      ifelse tiempo-infectado > duración-infección [
        set infectado? false
        set pcolor white
        set tiempo-infectado 0
      ]
      [
        set pcolor 38
        set tiempo-infectado (tiempo-infectado + 1)
      ]
    ]
  ]
end

;; Infectar a las personas encimas de patches
to infectar-persona-patch
  ask patches with [infectado?][
    ask (turtles-on self ) with [ not contagiado? and not recuperado? ][
      if random-float 100 < probabilidad-patch [
        contagiarse
      ]
    ]
  ]
end

;; Cambiar color correspondiente
to actualizar-display
  ask turtles
    [ set color ifelse-value contagiado? [ red ] [ ifelse-value recuperado? [ blue ] [ green ] ] ]
end


;; Turtles move about at random.
to mover
  let x0 xcor
  let y0 ycor
  let rt0 random 100
  let lt0 random 100

  rt rt0
  lt lt0
  fd 1
  if not gis:contained-by? self contorno [
    setxy x0 y0
  ]
end
@#$#@#$#@
GRAPHICS-WINDOW
455
30
1968
1044
-1
-1
5.0
1
10
1
1
1
0
1
1
1
-150
150
-100
100
1
1
1
ticks
30.0

SLIDER
90
110
284
143
tasa-infección
tasa-infección
0.0
99.0
98.0
1.0
1
%
HORIZONTAL

BUTTON
110
300
180
335
NIL
setup
NIL
1
T
OBSERVER
NIL
NIL
NIL
NIL
1

BUTTON
195
300
267
336
NIL
correr
T
1
T
OBSERVER
NIL
NIL
NIL
NIL
0

PLOT
50
435
425
670
Población
Horas
Personas
0.0
52.0
0.0
200.0
true
true
"" ""
PENS
"Contagiado" 1.0 0 -2674135 true "" "plot count turtles with [ contagiado? ]"
"Recuperado" 1.0 0 -7500403 true "" "plot count turtles with [ recuperado? ]"
"Susceptibles" 1.0 0 -10899396 true "" "plot count turtles with [ not contagiado? and not recuperado? ]"
"Muertos" 1.0 0 -7858858 true "" "plot muertos"

SLIDER
90
70
287
103
tamaño-de-población
tamaño-de-población
10
10000
500.0
1
1
NIL
HORIZONTAL

MONITOR
70
370
160
415
NIL
%infectados
1
1
11

MONITOR
185
370
275
415
NIL
%inmune
1
1
11

MONITOR
305
370
395
415
years
ticks / 52
1
1
11

SLIDER
90
150
285
183
init-infectados
init-infectados
0
100
20.0
1
1
NIL
HORIZONTAL

PLOT
50
685
425
920
Contagiados por dia
Días
Población
0.0
10.0
0.0
10.0
true
false
"" ""
PENS
"Contagiados" 1.0 1 -13840069 true "" "plot max (list (dia_actual - dia_previo) 0)"

SLIDER
90
230
352
263
duración-virus-patch
duración-virus-patch
1
20
20.0
0.5
1
horas
HORIZONTAL

BUTTON
280
300
350
335
paso
correr
NIL
1
T
OBSERVER
NIL
NIL
NIL
NIL
1

PLOT
50
945
425
1180
Muertos por día
Horas
Personas
0.0
10.0
0.0
10.0
true
false
"" ""
PENS
"default" 1.0 1 -16777216 true "" "plot (max (list (muertos_actual - muertos_previo) 0))"

SLIDER
90
30
285
63
probabilidad-patch
probabilidad-patch
0
15
8.0
0.5
1
NIL
HORIZONTAL

SLIDER
90
190
262
223
radio-infección
radio-infección
0
2
1.0
0.1
1
NIL
HORIZONTAL

@#$#@#$#@
## WHAT IS IT?

This model simulates the transmission and perpetuation of a virus in a human population.

Ecological biologists have suggested a number of factors which may influence the survival of a directly transmitted virus within a population. (Yorke, et al. "Seasonality and the requirements for perpetuation and eradication of viruses in populations." Journal of Epidemiology, volume 109, pages 103-123)

## HOW IT WORKS

The model is initialized with 150 people, of which 10 are infected.  People move randomly about the world in one of three states: healthy but susceptible to infection (green), sick and infectious (red), and healthy and immune (gray). People may die of infection or old age.  When the population dips below the environment's "carrying capacity" (set at 300 in this model) healthy people may produce healthy (but susceptible) offspring.

Some of these factors are summarized below with an explanation of how each one is treated in this model.

### The density of the population

Population density affects how often infected, immune and susceptible individuals come into contact with each other. You can change the size of the initial population through the NUMBER-PEOPLE slider.

### Population turnover

As individuals die, some who die will be infected, some will be susceptible and some will be immune.  All the new individuals who are born, replacing those who die, will be susceptible.  People may die from the virus, the chances of which are determined by the slider CHANCE-RECOVER, or they may die of old age.

In this model, people die of old age at the age of 50 years.  Reproduction rate is constant in this model.  Each turn, if the carrying capacity hasn't been reached, every healthy individual has a 1% chance to reproduce.

### Degree of immunity

If a person has been infected and recovered, how immune are they to the virus?  We often assume that immunity lasts a lifetime and is assured, but in some cases immunity wears off in time and immunity might not be absolutely secure.  In this model, immunity is secure, but it only lasts for a year.

### Infectiousness (or transmissibility)

How easily does the virus spread?  Some viruses with which we are familiar spread very easily.  Some viruses spread from the smallest contact every time.  Others (the HIV virus, which is responsible for AIDS, for example) require significant contact, perhaps many times, before the virus is transmitted.  In this model, infectiousness is determined by the INFECTIOUSNESS slider.

### Duration of infectiousness

How long is a person infected before they either recover or die?  This length of time is essentially the virus's window of opportunity for transmission to new hosts. In this model, duration of infectiousness is determined by the DURATION slider.

### Hard-coded parameters

Four important parameters of this model are set as constants in the code (See `setup-constants` procedure). They can be exposed as sliders if desired. The turtles’ lifespan is set to 50 years, the carrying capacity of the world is set to 300, the duration of immunity is set to 52 weeks, and the birth-rate is set to a 1 in 100 chance of reproducing per tick when the number of people is less than the carrying capacity.

## HOW TO USE IT

Each "tick" represents a week in the time scale of this model.

The INFECTIOUSNESS slider determines how great the chance is that virus transmission will occur when an infected person and susceptible person occupy the same patch.  For instance, when the slider is set to 50, the virus will spread roughly once every two chance encounters.

The DURATION slider determines the number of weeks before an infected person either dies or recovers.

The CHANCE-RECOVER slider controls the likelihood that an infection will end in recovery/immunity.  When this slider is set at zero, for instance, the infection is always deadly.

The SETUP button resets the graphics and plots and randomly distributes NUMBER-PEOPLE in the view. All but 10 of the people are set to be green susceptible people and 10 red infected people (of randomly distributed ages).  The GO button starts the simulation and the plotting function.

The TURTLE-SHAPE chooser controls whether the people are visualized as person shapes or as circles.

Three output monitors show the percent of the population that is infected, the percent that is immune, and the number of years that have passed.  The plot shows (in their respective colors) the number of susceptible, infected, and immune people.  It also shows the number of individuals in the total population in blue.

## THINGS TO NOTICE

The factors controlled by the three sliders interact to influence how likely the virus is to thrive in this population.  Notice that in all cases, these factors must create a balance in which an adequate number of potential hosts remain available to the virus and in which the virus can adequately access those hosts.

Often there will initially be an explosion of infection since no one in the population is immune.  This approximates the initial "outbreak" of a viral infection in a population, one that often has devastating consequences for the humans concerned. Soon, however, the virus becomes less common as the population dynamics change.  What ultimately happens to the virus is determined by the factors controlled by the sliders.

Notice that viruses that are too successful at first (infecting almost everyone) may not survive in the long term.  Since everyone infected generally dies or becomes immune as a result, the potential number of hosts is often limited.  The exception to the above is when the DURATION slider is set so high that population turnover (reproduction) can keep up and provide new hosts.

## THINGS TO TRY

Think about how different slider values might approximate the dynamics of real-life viruses.  The famous Ebola virus in central Africa has a very short duration, a very high infectiousness value, and an extremely low recovery rate. For all the fear this virus has raised, how successful is it?  Set the sliders appropriately and watch what happens.

The HIV virus, which causes AIDS, has an extremely long duration, an extremely low recovery rate, but an extremely low infectiousness value.  How does a virus with these slider values fare in this model?

## EXTENDING THE MODEL

Add additional sliders controlling the carrying capacity of the world (how many people can be in the world at one time), the average lifespan of the people and their birth-rate.

Build a similar model simulating viral infection of a non-human host with very different reproductive rates, lifespans, and population densities.

Add a slider controlling how long immunity lasts. You could also make immunity imperfect, so that immune turtles still have a small chance of getting infected. This chance could get higher over time.

## VISUALIZATION

The circle visualization of the model comes from guidelines presented in
Kornhauser, D., Wilensky, U., & Rand, W. (2009). http://ccl.northwestern.edu/papers/2009/Kornhauser,Wilensky&Rand_DesignGuidelinesABMViz.pdf.

At the lowest level, perceptual impediments arise when we exceed the limitations of our low-level visual system. Visual features that are difficult to distinguish can disable our pre-attentive processing capabilities. Pre-attentive processing can be hindered by other cognitive phenomena such as interference between visual features (Healey 2006).

The circle visualization in this model is supposed to make it easier to see when agents interact because overlap is easier to see between circles than between the "people" shapes. In the circle visualization, the circles merge to create new compound shapes. Thus, it is easier to perceive new compound shapes in the circle visualization.
Does the circle visualization make it easier for you to see what is happening?

## RELATED MODELS

* HIV
* Virus on a Network

## CREDITS AND REFERENCES

This model can show an alternate visualization of the Virus model using circles to represent the people. It uses visualization techniques as recommended in the paper:

Kornhauser, D., Wilensky, U., & Rand, W. (2009). Design guidelines for agent based model visualization. Journal of Artificial Societies and Social Simulation, JASSS, 12(2), 1.

## HOW TO CITE

If you mention this model or the NetLogo software in a publication, we ask that you include the citations below.

For the model itself:

* Wilensky, U. (1998).  NetLogo Virus model.  http://ccl.northwestern.edu/netlogo/models/Virus.  Center for Connected Learning and Computer-Based Modeling, Northwestern University, Evanston, IL.

Please cite the NetLogo software as:

* Wilensky, U. (1999). NetLogo. http://ccl.northwestern.edu/netlogo/. Center for Connected Learning and Computer-Based Modeling, Northwestern University, Evanston, IL.

## COPYRIGHT AND LICENSE

Copyright 1998 Uri Wilensky.

![CC BY-NC-SA 3.0](http://ccl.northwestern.edu/images/creativecommons/byncsa.png)

This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 3.0 License.  To view a copy of this license, visit https://creativecommons.org/licenses/by-nc-sa/3.0/ or send a letter to Creative Commons, 559 Nathan Abbott Way, Stanford, California 94305, USA.

Commercial licenses are also available. To inquire about commercial licenses, please contact Uri Wilensky at uri@northwestern.edu.

This model was created as part of the project: CONNECTED MATHEMATICS: MAKING SENSE OF COMPLEX PHENOMENA THROUGH BUILDING OBJECT-BASED PARALLEL MODELS (OBPML).  The project gratefully acknowledges the support of the National Science Foundation (Applications of Advanced Technologies Program) -- grant numbers RED #9552950 and REC #9632612.

This model was converted to NetLogo as part of the projects: PARTICIPATORY SIMULATIONS: NETWORK-BASED DESIGN FOR SYSTEMS LEARNING IN CLASSROOMS and/or INTEGRATED SIMULATION AND MODELING ENVIRONMENT. The project gratefully acknowledges the support of the National Science Foundation (REPP & ROLE programs) -- grant numbers REC #9814682 and REC-0126227. Converted from StarLogoT to NetLogo, 2001.

<!-- 1998 2001 -->
@#$#@#$#@
default
true
0
Polygon -7500403 true true 150 5 40 250 150 205 260 250

airplane
true
0
Polygon -7500403 true true 150 0 135 15 120 60 120 105 15 165 15 195 120 180 135 240 105 270 120 285 150 270 180 285 210 270 165 240 180 180 285 195 285 165 180 105 180 60 165 15

arrow
true
0
Polygon -7500403 true true 150 0 0 150 105 150 105 293 195 293 195 150 300 150

box
false
0
Polygon -7500403 true true 150 285 285 225 285 75 150 135
Polygon -7500403 true true 150 135 15 75 150 15 285 75
Polygon -7500403 true true 15 75 15 225 150 285 150 135
Line -16777216 false 150 285 150 135
Line -16777216 false 150 135 15 75
Line -16777216 false 150 135 285 75

bug
true
0
Circle -7500403 true true 96 182 108
Circle -7500403 true true 110 127 80
Circle -7500403 true true 110 75 80
Line -7500403 true 150 100 80 30
Line -7500403 true 150 100 220 30

butterfly
true
0
Polygon -7500403 true true 150 165 209 199 225 225 225 255 195 270 165 255 150 240
Polygon -7500403 true true 150 165 89 198 75 225 75 255 105 270 135 255 150 240
Polygon -7500403 true true 139 148 100 105 55 90 25 90 10 105 10 135 25 180 40 195 85 194 139 163
Polygon -7500403 true true 162 150 200 105 245 90 275 90 290 105 290 135 275 180 260 195 215 195 162 165
Polygon -16777216 true false 150 255 135 225 120 150 135 120 150 105 165 120 180 150 165 225
Circle -16777216 true false 135 90 30
Line -16777216 false 150 105 195 60
Line -16777216 false 150 105 105 60

car
false
0
Polygon -7500403 true true 300 180 279 164 261 144 240 135 226 132 213 106 203 84 185 63 159 50 135 50 75 60 0 150 0 165 0 225 300 225 300 180
Circle -16777216 true false 180 180 90
Circle -16777216 true false 30 180 90
Polygon -16777216 true false 162 80 132 78 134 135 209 135 194 105 189 96 180 89
Circle -7500403 true true 47 195 58
Circle -7500403 true true 195 195 58

circle
false
0
Circle -7500403 true true 0 0 300

circle 2
false
0
Circle -7500403 true true 0 0 300
Circle -16777216 true false 30 30 240

cow
false
0
Polygon -7500403 true true 200 193 197 249 179 249 177 196 166 187 140 189 93 191 78 179 72 211 49 209 48 181 37 149 25 120 25 89 45 72 103 84 179 75 198 76 252 64 272 81 293 103 285 121 255 121 242 118 224 167
Polygon -7500403 true true 73 210 86 251 62 249 48 208
Polygon -7500403 true true 25 114 16 195 9 204 23 213 25 200 39 123

cylinder
false
0
Circle -7500403 true true 0 0 300

dot
false
0
Circle -7500403 true true 90 90 120

face happy
false
0
Circle -7500403 true true 8 8 285
Circle -16777216 true false 60 75 60
Circle -16777216 true false 180 75 60
Polygon -16777216 true false 150 255 90 239 62 213 47 191 67 179 90 203 109 218 150 225 192 218 210 203 227 181 251 194 236 217 212 240

face neutral
false
0
Circle -7500403 true true 8 7 285
Circle -16777216 true false 60 75 60
Circle -16777216 true false 180 75 60
Rectangle -16777216 true false 60 195 240 225

face sad
false
0
Circle -7500403 true true 8 8 285
Circle -16777216 true false 60 75 60
Circle -16777216 true false 180 75 60
Polygon -16777216 true false 150 168 90 184 62 210 47 232 67 244 90 220 109 205 150 198 192 205 210 220 227 242 251 229 236 206 212 183

fish
false
0
Polygon -1 true false 44 131 21 87 15 86 0 120 15 150 0 180 13 214 20 212 45 166
Polygon -1 true false 135 195 119 235 95 218 76 210 46 204 60 165
Polygon -1 true false 75 45 83 77 71 103 86 114 166 78 135 60
Polygon -7500403 true true 30 136 151 77 226 81 280 119 292 146 292 160 287 170 270 195 195 210 151 212 30 166
Circle -16777216 true false 215 106 30

flag
false
0
Rectangle -7500403 true true 60 15 75 300
Polygon -7500403 true true 90 150 270 90 90 30
Line -7500403 true 75 135 90 135
Line -7500403 true 75 45 90 45

flower
false
0
Polygon -10899396 true false 135 120 165 165 180 210 180 240 150 300 165 300 195 240 195 195 165 135
Circle -7500403 true true 85 132 38
Circle -7500403 true true 130 147 38
Circle -7500403 true true 192 85 38
Circle -7500403 true true 85 40 38
Circle -7500403 true true 177 40 38
Circle -7500403 true true 177 132 38
Circle -7500403 true true 70 85 38
Circle -7500403 true true 130 25 38
Circle -7500403 true true 96 51 108
Circle -16777216 true false 113 68 74
Polygon -10899396 true false 189 233 219 188 249 173 279 188 234 218
Polygon -10899396 true false 180 255 150 210 105 210 75 240 135 240

house
false
0
Rectangle -7500403 true true 45 120 255 285
Rectangle -16777216 true false 120 210 180 285
Polygon -7500403 true true 15 120 150 15 285 120
Line -16777216 false 30 120 270 120

leaf
false
0
Polygon -7500403 true true 150 210 135 195 120 210 60 210 30 195 60 180 60 165 15 135 30 120 15 105 40 104 45 90 60 90 90 105 105 120 120 120 105 60 120 60 135 30 150 15 165 30 180 60 195 60 180 120 195 120 210 105 240 90 255 90 263 104 285 105 270 120 285 135 240 165 240 180 270 195 240 210 180 210 165 195
Polygon -7500403 true true 135 195 135 240 120 255 105 255 105 285 135 285 165 240 165 195

line
true
0
Line -7500403 true 150 0 150 300

line half
true
0
Line -7500403 true 150 0 150 150

pentagon
false
0
Polygon -7500403 true true 150 15 15 120 60 285 240 285 285 120

person
false
0
Circle -7500403 true true 110 5 80
Polygon -7500403 true true 105 90 120 195 90 285 105 300 135 300 150 225 165 300 195 300 210 285 180 195 195 90
Rectangle -7500403 true true 127 79 172 94
Polygon -7500403 true true 195 90 240 150 225 180 165 105
Polygon -7500403 true true 105 90 60 150 75 180 135 105

plant
false
0
Rectangle -7500403 true true 135 90 165 300
Polygon -7500403 true true 135 255 90 210 45 195 75 255 135 285
Polygon -7500403 true true 165 255 210 210 255 195 225 255 165 285
Polygon -7500403 true true 135 180 90 135 45 120 75 180 135 210
Polygon -7500403 true true 165 180 165 210 225 180 255 120 210 135
Polygon -7500403 true true 135 105 90 60 45 45 75 105 135 135
Polygon -7500403 true true 165 105 165 135 225 105 255 45 210 60
Polygon -7500403 true true 135 90 120 45 150 15 180 45 165 90

square
false
0
Rectangle -7500403 true true 30 30 270 270

square 2
false
0
Rectangle -7500403 true true 30 30 270 270
Rectangle -16777216 true false 60 60 240 240

star
false
0
Polygon -7500403 true true 151 1 185 108 298 108 207 175 242 282 151 216 59 282 94 175 3 108 116 108

target
false
0
Circle -7500403 true true 0 0 300
Circle -16777216 true false 30 30 240
Circle -7500403 true true 60 60 180
Circle -16777216 true false 90 90 120
Circle -7500403 true true 120 120 60

tree
false
0
Circle -7500403 true true 118 3 94
Rectangle -6459832 true false 120 195 180 300
Circle -7500403 true true 65 21 108
Circle -7500403 true true 116 41 127
Circle -7500403 true true 45 90 120
Circle -7500403 true true 104 74 152

triangle
false
0
Polygon -7500403 true true 150 30 15 255 285 255

triangle 2
false
0
Polygon -7500403 true true 150 30 15 255 285 255
Polygon -16777216 true false 151 99 225 223 75 224

truck
false
0
Rectangle -7500403 true true 4 45 195 187
Polygon -7500403 true true 296 193 296 150 259 134 244 104 208 104 207 194
Rectangle -1 true false 195 60 195 105
Polygon -16777216 true false 238 112 252 141 219 141 218 112
Circle -16777216 true false 234 174 42
Rectangle -7500403 true true 181 185 214 194
Circle -16777216 true false 144 174 42
Circle -16777216 true false 24 174 42
Circle -7500403 false true 24 174 42
Circle -7500403 false true 144 174 42
Circle -7500403 false true 234 174 42

turtle
true
0
Polygon -10899396 true false 215 204 240 233 246 254 228 266 215 252 193 210
Polygon -10899396 true false 195 90 225 75 245 75 260 89 269 108 261 124 240 105 225 105 210 105
Polygon -10899396 true false 105 90 75 75 55 75 40 89 31 108 39 124 60 105 75 105 90 105
Polygon -10899396 true false 132 85 134 64 107 51 108 17 150 2 192 18 192 52 169 65 172 87
Polygon -10899396 true false 85 204 60 233 54 254 72 266 85 252 107 210
Polygon -7500403 true true 119 75 179 75 209 101 224 135 220 225 175 261 128 261 81 224 74 135 88 99

wheel
false
0
Circle -7500403 true true 3 3 294
Circle -16777216 true false 30 30 240
Line -7500403 true 150 285 150 15
Line -7500403 true 15 150 285 150
Circle -7500403 true true 120 120 60
Line -7500403 true 216 40 79 269
Line -7500403 true 40 84 269 221
Line -7500403 true 40 216 269 79
Line -7500403 true 84 40 221 269

x
false
0
Polygon -7500403 true true 270 75 225 30 30 225 75 270
Polygon -7500403 true true 30 75 75 30 270 225 225 270
@#$#@#$#@
NetLogo 6.1.1
@#$#@#$#@
@#$#@#$#@
@#$#@#$#@
@#$#@#$#@
@#$#@#$#@
default
0.0
-0.2 0 0.0 1.0
0.0 1 1.0 0.0
0.2 0 0.0 1.0
link direction
true
0
Line -7500403 true 150 150 90 180
Line -7500403 true 150 150 210 180
@#$#@#$#@
1
@#$#@#$#@
