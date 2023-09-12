.. loserCE documentation master file, created by
   sphinx-quickstart on Tue Apr 18 08:15:38 2023.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

Welcome to loserCE's documentation!
===================================

.. toctree::
   :maxdepth: 2
   :caption: Contents:

   z_chapter




Indices and tables
==================

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`

Docs
=================

.. namespace '::' className
.. doxygenclass:: chessInterface::uci
   :project: loserCE
   :members:


Product description
===================

Sytem context 
=============


lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod
tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At
vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren,
no sea takimata sanctus est Lorem ipsum dolor sit amet. Lorem ipsum dolor sit
amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut
labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam


.. code-block:: cpp
   :linenos:

   #include <iostream>
   #include <string>

   int main()
   {
       std::cout << "Hello World!" << std::endl;
       return 0;
   }

.. uml::

   class command {
       +execute()
   }
   class concreteCommand {
       +execute()
   }
   class invoker {
       +storeCommand()
   }
   class receiver {
       +action()
   }

   invoker -> command: execute()
   command -> receiver: action()
   receiver -> invoker: storeCommand()
   

Temperature conversion
======================

Below is the formula for converting ADC to Ohm:

input values are:

- :math:`ADC` - raw ADC value
- :math:`R_{ref}` - reference resistor value
- :math:`R_{therm}` - thermistor value
- :math:`ADC_{max}` - maximum ADC value 32767

.. math::

   celsius = (fahrenheit - 32) \cdot \frac{5}{9}

   R_{T} = \frac{ ADC(T) }{ ADC_{max}} \cdot R_{ref}

   
PT100
+++++


.. math::


    
    a = 3.90830 \cdot 10^3 \\\\
    b = -5.77500 \cdot 10^7 \\\\ 
    c=
    \begin{cases}
    -4.18301 \cdot 10^12 \quad for -200^{\circ} \le T \le 0^{\circ} \\\\ 
    0 \quad for 0^{\circ} < T +850^{\circ}
    \end{cases}\\\\ \\\\

   

    R(T) = R_{0}(1 + aT + bT^2 + c(T - 100)T^3)

    

The PT100 curve is really linear, so we can use a simple linear equation to 
calculate the temperature. We are majorly interested in a high accuracy in
the temperature range from 50 to 120 degrees Celsius, so we can use the 
following equation. To do so we simply place a linear equation between the
temperature points 50 and 120 degrees Celsius and use it to calculate the
temperature.

.. math::

    R_{s}(T) = aT + b  \\\\

    a = \frac{R(120) - R_(50)}{120 - 50} \\\\
    a = \frac{26.670875}{70} = 0.3810125  \\\\
    b = 100

with this curve we can oberve a slight deviation around -0.3 degrees celsius 
from the linear in the range between 50 and 120 degrees Celsius. To 
compensate this we simply increase :math:`b` by 0.393 (tweaked by hand 
to minimize the deviation).

.. math::

    R_{s}(T) = a*T + b  \\\\

    a = \frac{R(120) - R_(50)}{120 - 50}  \\\\
    a = \frac{26.670875}{70} = 0.3810125  \\\\
    b = 100.393

Finally we have to combine the two equations to get the temperature from the
ADC values.

.. math::

    T(R) = \frac{R_s(T)) - b}{a}  \\\\
    R_{T} = \frac{ ADC(T) }{ ADC_{max}} \cdot R_{ref} \\\\  \\\\


    T(R) = \frac{\frac{ ADC }{ ADC_{max}} \cdot R_{ref} - b}{a} 


