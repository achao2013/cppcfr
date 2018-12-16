import sys
sys.path.insert(0,'build/lib/')
import os
sys.path.insert(0,os.path.realpath('./python/pycfr'))
import copy
from card import Card
from pokerstrategy import *
from pokergames import *
import pycppcfr as pcc
from card import Card

def cmp_hole(hole1,hole2):
    for i in range(len(hole1)):
        if hole1[i]>hole2[i]:
            return 1
        elif hole1[i]<hole2[i]:
            return -1
        else:
            if i==len(hole1)-1:
                return 0
            else:
                continue

def depthTraversal0(root):
    if root:
        if type(root) is TerminalNode:
            holecards = list(root.holecards)
            newholes = []
            for i in range(len(holecards)):
                newholes.append([])
                for j in range(len(list(holecards[i]))):
                    holecards[i][j] = list(holecards[i][j])
                    for k in range(len(list(holecards[i][j]))):
                        holecards[i][j][k] = pcc.Card(holecards[i][j][k].rank, holecards[i][j][k].suit)
                    newholes[i].append(tuple(sorted(holecards[i][j])))
                holecards[i]=tuple(sorted(newholes[i]))
            holecards = tuple(sorted(holecards, cmp=cmp_hole))
            #print type(pcc.Card(holecards[0][0][0].rank, holecards[0][0][0].suit))
            board = list(root.board)
            for i in range(len(board)):
                board[i] = pcc.Card(board[i].rank, board[i].suit)
            board = tuple(board)
            payoffs = root.payoffs
            bet_history = root.bet_history
            for k,v in payoffs.items():
                payoffs[k] = list(v)
                newk = list(k)
                for i in range(len(newk)):
                    newk[i] = list(newk[i])
                    for j in range(len(newk[i])):
                        newk[i][j] = pcc.Card(newk[i][j].rank, newk[i][j].suit)
                    newk[i] = tuple(newk[i])
                newk = tuple(newk)
                payoffs[newk] = payoffs.pop(k)
            if terminalinfo0.has_key(bet_history):
                if terminalinfo0[bet_history].has_key(holecards):
                    terminalinfo0[bet_history][holecards][board] = payoffs
                else:
                    terminalinfo0[bet_history][holecards] = {board:payoffs}
            else:
                terminalinfo0[bet_history] = {}
                terminalinfo0[bet_history][holecards] = {board:payoffs}
            #for k,v in payoffs.items():
            #    print type(k),type(k[0]), type(v)
            #print root.holecards," ",root.board," ",root.bet_history," payoffs:",root.payoffs
        elif(root.children is not None):
            for i in range(len(root.children)):
                depthTraversal0(root.children[i])

def depthTraversal(root):
    if root:
        if type(root) is pcc.TerminalNode:
            holecards = list(root.holecards)
            newholes = []
            for i in range(len(holecards)):
                newholes.append([])
                for j in range(len(list(holecards[i]))):
                    newholes[i].append(tuple(sorted(holecards[i][j])))
                holecards[i]=tuple(sorted(newholes[i]))
            holecards = tuple(sorted(holecards, cmp=cmp_hole))
            #print type(holecards[0][0][0])
            board = tuple(root.board)
            payoffs = dict(root.payoffs)
            bet_history = str(root.bet_history)
            if terminalinfo.has_key(bet_history):
                if terminalinfo[bet_history].has_key(holecards):
                    terminalinfo[bet_history][holecards][board] = payoffs
                else:
                    terminalinfo[bet_history][holecards] = {board:payoffs}
            else:
                terminalinfo[bet_history] = {}
                terminalinfo[bet_history][holecards] = {board:payoffs}
            #print holecards," ",board," ",bet_history," payoffs:", payoffs
            for k,v in payoffs.items():
                payoffs[k] = list(v)
                newk = list(k)
                for i in range(len(newk)):
                    newk[i] = tuple(newk[i])
                newk = tuple(newk)
                payoffs[newk] = payoffs.pop(k)
            #for k,v in payoffs.items():
            #    print type(k), type(k[0]), type(v)

        elif(root.children is not None):
            for i in range(len(root.children)):
                depthTraversal(root.children[i])

########## python ###########
print "original python......................"
leduc_rules = leduc_rules()
leduc_gt = PublicTree(leduc_rules)
leduc_gt.build()

terminalinfo0 = {}
depthTraversal0(leduc_gt.root)
#print terminalinfo0

############ c++ call ##########
print "c++ call........................."
gr = pcc.leduc_rules()
#print gr
pt = pcc.PublicTree(gr)
pt.build()
print type(pt.root)
terminalinfo = {}
depthTraversal(pt.root)
#print terminalinfo

############ python vs c++ #################
outer = dict.fromkeys([x for x in terminalinfo0 if x not in terminalinfo])
print outer

for k0,v0 in terminalinfo0.items():
    for k1,v1 in v0.items():
        for k2,v2 in v1.items():
            if (not terminalinfo.has_key(k0)):
                print "no bet_history:",k0
                continue
            else:
                #print v0.keys(),terminalinfo[k0].keys()
                pass
            #print k1[0] , terminalinfo[k0].keys()[0][0], k1[0] == terminalinfo[k0].keys()[0][0]
            if (not terminalinfo[k0].has_key(k1)):
                all_hc = terminalinfo[k0].keys()
                for hc in all_hc:
                    print k1,' ',hc,' ',k1==hc,' ', terminalinfo[k0].has_key(k1)
                continue
            if (not terminalinfo[k0][k1].has_key(k2)):
                print "no board:",type(k2), ' ',type(terminalinfo[k0][k1].keys()[0])
                continue
            if terminalinfo[k0][k1][k2] != v2:
                print "wrong payoffs",v2, terminalinfo[k0][k1][k2]
            else:
                #print v2,' == ',terminalinfo[k0][k1][k2]
                pass
print "check over"
