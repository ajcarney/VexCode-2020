#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import time

class stopwatch:
	
	def __init__(self):
		self.beginning = None
		
	def start(self):
		"""
		starts stopwatch
		"""
		self.beginning = time.time()
		
	def stop(self):
		"""
		stops stopwatch and returns
		time elapsed
		"""
		end = time.time()
		timeElapsed = end - self.beginning
		
		return timeElapsed
