import { motion } from 'framer-motion';
import { useInView } from 'react-intersection-observer';
import { FaWindowMaximize, FaPaintBrush, FaCube, FaCode } from 'react-icons/fa';

const modules = [
  {
    name: 'Window Management',
    icon: <FaWindowMaximize className="w-8 h-8" />,
    description: 'GLFW window handling, OpenGL context management, and input processing',
    connections: ['Renderer', 'Scene'],
  },
  {
    name: 'Renderer',
    icon: <FaPaintBrush className="w-8 h-8" />,
    description: 'OpenGL state management, shader programs, and performance monitoring',
    connections: ['Scene', 'GameObjects'],
  },
  {
    name: 'Scene',
    icon: <FaCube className="w-8 h-8" />,
    description: 'Object lifecycle management, collision detection, and ray-object interactions',
    connections: ['GameObjects'],
  },
  {
    name: 'GameObjects',
    icon: <FaCode className="w-8 h-8" />,
    description: 'Base GameObject class with specialized implementations for different object types',
    connections: [],
  },
];

const ModuleCard = ({ name, icon, description, connections, index }) => {
  const [ref, inView] = useInView({
    triggerOnce: true,
    threshold: 0.1,
  });

  return (
    <motion.div
      ref={ref}
      initial={{ opacity: 0, y: 50 }}
      animate={inView ? { opacity: 1, y: 0 } : { opacity: 0, y: 50 }}
      transition={{ duration: 0.5, delay: index * 0.1 }}
      className="glass p-6 rounded-xl hover:scale-105 transition-transform duration-300 relative"
    >
      <div className="text-accent mb-4">{icon}</div>
      <h3 className="text-xl font-display font-bold mb-2">{name}</h3>
      <p className="text-gray-300 text-sm mb-4">{description}</p>
    </motion.div>
  );
};

const Architecture = () => {
  return (
    <section id="architecture" className="section">
      <div className="container">
        <motion.div
          initial={{ opacity: 0, y: 20 }}
          whileInView={{ opacity: 1, y: 0 }}
          transition={{ duration: 0.5 }}
          viewport={{ once: true }}
          className="text-center mb-16"
        >
          <h2 className="text-4xl md:text-5xl font-display font-bold mb-4">
            Architecture
          </h2>
          <p className="text-gray-300 text-lg">
            Clean and modular design for optimal performance
          </p>
        </motion.div>

        <div className="relative">
          <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-4 gap-8">
            {modules.map((module, index) => (
              <ModuleCard key={index} {...module} index={index} />
            ))}
          </div>
        </div>
      </div>
    </section>
  );
};

export default Architecture; 