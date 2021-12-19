
# TODO delete
# def delete(self, x: Node):
#     if x.left and x.right:
#         swapX = self.max(x.right)
#         swapX.key, x.key = x.key, swapX.key
#         swapX.value, x.value = x.value, swapX.value
#         _updateParent(swapX, swapX.right)
#         delete swapX
#     else if x.left:
#         newX = x.left
#         _updateParent(x, newX)
#         delete x
#     else if x.right:
#         newX = x.right
#         _moveNode(x, newX)
#         delete x
#     else:
#         _updateParent(x, null)
#         delete x
