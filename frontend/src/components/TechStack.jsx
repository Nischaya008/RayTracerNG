import { motion } from 'framer-motion';
import { useInView } from 'react-intersection-observer';
import { SiCplusplus, SiOpengl, SiGithub, SiCmake } from 'react-icons/si';
import { FaCode, FaWindowMaximize, FaCalculator, FaTools } from 'react-icons/fa';

const technologies = [
  {
    name: 'C++17',
    icon: <SiCplusplus className="w-12 h-12" />,
    description: 'Modern C++ for high-performance graphics programming',
  },
  {
    name: 'OpenGL 4.5+',
    icon: <SiOpengl className="w-12 h-12" />,
    description: 'Advanced graphics API with shader-based rendering',
  },
  {
    name: 'GLFW',
    icon: <FaWindowMaximize className="w-12 h-12" />,
    description: 'Cross-platform window management and input handling',
  },
  {
    name: 'GLM',
    icon: <FaCalculator className="w-12 h-12" />,
    description: 'Mathematics library for graphics applications',
  },
  {
    name: 'ImGui',
    icon: <FaTools className="w-12 h-12" />,
    description: 'Immediate mode GUI with docking support',
  },
  {
    name: 'CMake',
    icon: <SiCmake className="w-12 h-12" />,
    description: 'Modern build system for cross-platform development',
  },
];

const TechCard = ({ name, icon, description, index }) => {
  const [ref, inView] = useInView({
    triggerOnce: true,
    threshold: 0.1,
  });

  return (
    <motion.div
      ref={ref}
      initial={{ opacity: 0, scale: 0.8 }}
      animate={inView ? { opacity: 1, scale: 1 } : { opacity: 0, scale: 0.8 }}
      transition={{ duration: 0.5, delay: index * 0.1 }}
      className="glass p-6 rounded-xl text-center hover:scale-105 transition-transform duration-300"
    >
      <div className="text-accent mb-4 flex justify-center">{icon}</div>
      <h3 className="text-xl font-display font-bold mb-2">{name}</h3>
      <p className="text-gray-300 text-sm">{description}</p>
    </motion.div>
  );
};

const TechStack = () => {
  return (
    <section id="tech stack" className="section bg-secondary">
      <div className="container">
        <motion.div
          initial={{ opacity: 0, y: 20 }}
          whileInView={{ opacity: 1, y: 0 }}
          transition={{ duration: 0.5 }}
          viewport={{ once: true }}
          className="text-center mb-16"
        >
          <h2 className="text-4xl md:text-5xl font-display font-bold mb-4">
            Tech Stack
          </h2>
          <p className="text-gray-300 text-lg">
            Built with modern technologies for optimal performance
          </p>
        </motion.div>

        <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-3 gap-8">
          {technologies.map((tech, index) => (
            <TechCard key={index} {...tech} index={index} />
          ))}
        </div>
      </div>
    </section>
  );
};

export default TechStack; 