#!/usr/bin/env python3
# -*- coding: utf-8 -*-


def main(field):
    """
    creates field elements
    """
    for num in range(0,6): #draw white line auton line
        field[2][num].drawRectangle(vertexes=[[47,0],[47,47],[47,47],[47,0]], color="#ffffff", width=10, outline="#ffffff")

    for num in range(0,6): #draw red alliance starting line
        field[4][num].drawRectangle(vertexes=[[47,0],[47,47],[47,47],[47,0]], color="#ffffff", width=7, outline="#ffffff")

    for num in range(0,6): #draw blue alliance starting line
        field[0][num].drawRectangle(vertexes=[[47,0],[47,47],[47,47],[47,0]], color="#ffffff", width=7, outline="#ffffff")
    
    field[0][3].drawRectangle(vertexes=[[0,0],[47,0],[47,0],[0,0]], color="#ffffff", width=10, outline="#ffffff")    
    field[5][3].drawRectangle(vertexes=[[0,0],[47,0],[47,0],[0,0]], color="#ffffff", width=10, outline="#ffffff")       
        
    #corner goal
    field[0][0].drawObjectFieldElementCircle(position=[11, 11], color='#919191', size=10)
    field[5][0].drawObjectFieldElementCircle(position=[36, 10], color='#919191', size=10)
    field[0][5].drawObjectFieldElementCircle(position=[11, 38], color='#919191', size=10)
    field[5][5].drawObjectFieldElementCircle(position=[36, 38], color='#919191', size=10)

    #cross goals
    field[3][0].drawObjectFieldElementCircle(position=[0, 10], color='#919191', size=10)
    field[3][3].drawObjectFieldElementCircle(position=[0, 0], color='#919191', size=10)
    field[3][5].drawObjectFieldElementCircle(position=[0, 38], color='#919191', size=10)
    
    field[0][3].drawObjectFieldElementCircle(position=[10, 0], color='#919191', size=10)   
    field[5][3].drawObjectFieldElementCircle(position=[36, 0], color='#919191', size=10)   
    
    
    #game objects
    
    #corner balls
    field[0][0].drawObjectFieldElementCircle(position=[24, 24], color='blue', size=7)
    field[0][5].drawObjectFieldElementCircle(position=[24, 24], color='blue', size=7)

    field[5][0].drawObjectFieldElementCircle(position=[24, 24], color='red', size=7)
    field[5][5].drawObjectFieldElementCircle(position=[24, 24], color='red', size=7)
    
    #middle balls
    field[3][2].drawObjectFieldElementCircle(position=[0, 29], color='blue', size=7)
    field[3][3].drawObjectFieldElementCircle(position=[18, 0], color='blue', size=7)

    field[3][3].drawObjectFieldElementCircle(position=[0, 18], color='red', size=7)
    field[2][3].drawObjectFieldElementCircle(position=[29, 0], color='red', size=7)
    
    #other balls
    field[3][1].drawObjectFieldElementCircle(position=[0, 24], color='blue', size=7)
    field[3][4].drawObjectFieldElementCircle(position=[0, 24], color='red', size=7)




