import copy
import random
import time

new_name = 0

class Category:
   def __init__(self, names, top):
      self.names = names
      self.top = top

   def containName(self, name):
      return name in self.names

class Agent:
   categories = [Category([], 1)]

   def split(self, a, b):
      global new_name

      for i in range(0, len(self.categories)):
         cat = self.categories[i]
            if (a < cat.top and b < cat.top):
               self.categories[i] = Category(list(cat.names), (a + b) / 2)
               self.categories.insert(i + 1, Category(list(cat.names), cat.top))
               self.categories[i].names.insert(0, new_name)
               self.categories[i + 1].names.insert(0, new_name + 1)
               new_name = new_name + 2
               return
            elif (a < cat.top or b < cat.top):
               return

    def getCategory(self, x):
         for cat in self.categories:
            if x < cat.top:
               return cat


def run(N, T, dmin):
   start = time.time()

   agents = [Agent()] * N

   for _ in range(0, int(T)):
      spk, lst = random.sample(agents, 2)

      a = random.uniform(0, 1)
      b = random.uniform(0, 1)
      while abs(a - b) > dmin:
         b = random.uniform(0, 1)

      spk.split(a, b)

      spkx = spk.getCategory(a)
      mrn = spkx.names[0]

      lstx = lst.getCategory(a)
      lsty = lst.getCategory(b)
      if lstx.containName(mrn) and not lsty.containName(mrn):
         spkx.names = [mrn]
         lstx.names = [mrn]
      else:
         lstx.names.append(mrn)

   end = time.time()
   print(end - start)

run(100, 100000, 0.05)
