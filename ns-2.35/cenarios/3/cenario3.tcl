# DEFINIÇÃO DE OPÇÕES WIRELESS. Fonte: Manual ns-2 em
#http://www.isi.edu/nsnam/ns/doc/index.html
if {$argc!=2} {
        #set tracefd [open cenario3a.csv w]
        puts "Erro! Indique o nome do arquivo e a quantidade de nós"
	exit
}
set val(chan) Channel/WirelessChannel ;# tipo de canal
set val(prop) Propagation/TwoRayGround ;# radio-propagation model
set val(netif) Phy/WirelessPhy ;# network interface type
set val(mac) Mac/802_11 ;# MAC type
set val(ifq) Queue/DropTail/PriQueue ;# interface queue type
set val(ll) LL ;# link layer type
set val(ant) Antenna/OmniAntenna ;# modelo de antena
set val(ifqlen) 50 ;# max packet in ifq
set val(nn) [lindex $argv 1]
#set val(nn) 100 ;# número de nós
set val(rp) DumbAgent ;# protocolo de roteamento (não existe roteamento)
set val(x) 1000 ;# X dimension of topography (metros)
set val(y) 1000 ;# Y dimension of topography (metros)
set val(stop) 5.0 ;# time of simulation end (segundos)

#PREPARANDO O SIMULADOR E OS TRACES
set ns [new Simulator]
set tracefd [open [lindex $argv 0] w]

#set windowVsTime2 [open cenario3a_wintr w]
#set namtrace [open cenario3a.nam w] 

#DEFININDO POTENCIA DO SINAL PARA LIMITAR ALCANCE DO LEITOR
$val(netif) set Pt_ 0.28
$val(netif) set RXThresh_ 2.12249e-07

#DESABILITANDO RTS/CTS POR NÃO FAZER PARTE DO PROTOCOLO RFID
$val(mac) set RTSThreshold_ 3000
#DEFININDO VELOCIDADE DOS CANAIS FORWARD(leitor-tag) E BACKWARD(tag-leitor)
$val(mac) set basicRate_ 500Kb
$val(mac) set dataRate_ 80Kb

#DEFININDO TRACES
$ns use-newtrace
$ns trace-all $tracefd
#$ns namtrace-all-wireless $namtrace $val(x) $val(y)

#DEFININDO TOPOLOGIA 20x20: Sala de aula
set topo [new Topography]

$topo load_flatgrid $val(x) $val(y)

create-god $val(nn)

set chan_1_ [new $val(chan)]

# CONFIGURANDO OS NÓS
$ns node-config -adhocRouting $val(rp) \
-llType $val(ll) \
-macType $val(mac) \
-ifqType $val(ifq) \
-ifqLen $val(ifqlen) \
-antType $val(ant) \
-propType $val(prop) \
-phyType $val(netif) \
-channelType $val(chan) \
-topoInstance $topo \
-agentTrace ON \
-routerTrace OFF \
-macTrace ON \
-movementTrace OFF

for {set i 0} {$i < $val(nn) } { incr i } {
	set n($i) [$ns node] 
}

# LOCALIZAÇÃO INICIAL DOS LEITORES
#LEITOR 1 - LEITOR DO CARRINHO DE COMPRAS
$n(0) set X_ 2
$n(0) set Y_ 2
$n(0) set Z_ 0.0

#LOCALIZAÇÃO ALEATÓRIA DAS TAGS
set rng1 [new RNG]
$rng1 seed 0
set rng2 [new RNG]
$rng2 seed 0

for {set i 1} {$i < $val(nn) } { incr i } {
      $n($i) set X_ [$rng1 uniform 3 8]
      $n($i) set Y_ [$rng2 uniform 3 8]
      $n($i) set Z_ 0.0
      #$ns at 0.0 "$n($i) label TAG"
}

#DEFININDO LABELS
$n(0) color red
$ns at 0.0 "$n(0) label LEITOR"

#Definindo conexão
set reader1 [new Agent/RfidReader]

for {set i 1} {$i < $val(nn) } { incr i } {
        set tag($i) [new Agent/RfidTag]
}


#DEFININDO IDENTIFICADORES DOS AGENTES
$reader1 set id_ 200
$reader1 set singularization_ 1
$reader1 set service_ 0
for {set i 1} {$i < $val(nn) } { incr i } {
        $tag($i) set tagEPC_ $i+10
	$tag($i) set time_ 0.5
}

#CONECTANDO NOS AOS AGENTES
for {set i 1} {$i < $val(nn) } { incr i } {
        $ns attach-agent $n($i) $tag($i)
}
$ns attach-agent $n(0) $reader1

#conectando nos
for {set i 1} {$i < $val(nn) } { incr i } {
        $ns connect $reader1 $tag($i)
}

#CONSULTANDO TAGS A CADA SEGUNDO
for {set i 0.0} {$i < $val(stop) } {set i [expr $i+0.1]} {
	$ns at $i "$reader1 query-tags"
}

#DEFININDO TAMANHO DOS NOS PARA O NAM
$ns initial_node_pos $n(0) 60
for {set i 1} {$i < $val(nn)} { incr i } {
	$ns initial_node_pos $n($i) 15
}

#DEFININDO MOVIMENTAÇÃO DOS NÓS (ALEATÓRIA)
#$ns at 0.0 "destination"
proc destination {} {
      global ns val n
      set time 1.0
      set rng3 [new RNG]
      $rng3 seed 0
      set rng4 [new RNG]
      $rng4 seed 0
      set rng5 [new RNG]
      $rng5 seed 0

      set now [$ns now]
      for {set i 1} {$i<$val(nn)} {incr i} {
            set xx1 [$rng3 uniform 0 10]
            set yy1 [$rng4 uniform 0 10]
	    #set zz1 [$rng5 uniform 0 10]
            #Entra na sala de aula
	    $ns at 0.0 "$n($i) setdest $xx1 $yy1 2.0"
      }
      #$ns at [expr $now+$time] "destination"
}

# Telling nodes when the simulation ends
for {set i 0} {$i < $val(nn) } { incr i } {
$ns at $val(stop) "$n($i) reset";
}

# ending nam and the simulation 
#$ns at $val(stop) "$ns nam-end-wireless $val(stop)"
$ns at $val(stop) "stop"
$ns at $val(stop) "puts \"end simulation\" ; $ns halt"



proc stop {} {
global ns tracefd
$ns flush-trace
close $tracefd
#close $namtrace
#exec nam cenario3a.nam &
}
$ns run
